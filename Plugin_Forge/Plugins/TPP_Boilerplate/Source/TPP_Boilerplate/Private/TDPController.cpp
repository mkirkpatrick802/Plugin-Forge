#include "TDPController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TargetableInterface.h"
#include "TPPCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

void ATDPController::BeginPlay()
{
	Super::BeginPlay();
}

void ATDPController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Cast<ATPPCharacter>(InPawn))
	{
		ControlledCharacter = Cast<ATPPCharacter>(InPawn);
		ControlledCharacter->SetupPlayerInputDelegate.AddDynamic(this, &ATDPController::SetupInput);

		// Camera Set Up
		CameraBoom = ControlledCharacter->GetCameraBoom();

		CameraBoom->TargetArmLength = ZoomMultiplier * ZoomStep;
		DesiredCameraRotation = CameraBoom->GetRelativeRotation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Cast Character!!"));
	}
}

void ATDPController::SetupInput()
{
	if (const auto InputSubsystem = ControlledCharacter->GetInputSubsystem())
	{
		InputSubsystem->AddMappingContext(ControllerMappingContext, 0);
	}

	if (const auto EnhancedInputComponent = ControlledCharacter->GetInputComponent())
	{
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATDPController::Zoom);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATDPController::Look);

		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ATDPController::FindNewTarget);
	}
}

void ATDPController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCameraZoom(DeltaSeconds);
	UpdateCameraRotation(DeltaSeconds);
	UpdateTargetList();
}

/*
 *		Camera Work
 */

void ATDPController::Zoom(const FInputActionValue& InputActionValue)
{
	const float ZoomDelta = InputActionValue.Get<float>();

	ZoomMultiplier += ZoomDelta;
	ZoomMultiplier = FMath::Clamp(ZoomMultiplier, ZoomBounds.X, ZoomBounds.Y);
}

void ATDPController::Look(const FInputActionValue& InputActionValue)
{
	if(!IsInputKeyDown(EKeys::MiddleMouseButton)) return;
	
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	const float HorizontalAxis = LookAxisVector.X * CurrentCameraSensitivity;
	DesiredCameraRotation.Yaw += HorizontalAxis;
}

void ATDPController::UpdateCameraZoom(const float DeltaSeconds) const
{
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ZoomStep * ZoomMultiplier,
													DeltaSeconds,ZoomSmoothingSpeed);
}

void ATDPController::UpdateCameraRotation(const float DeltaSeconds)
{
	const FRotator LerpedRotation = FMath::RInterpTo(CameraBoom->GetRelativeRotation(), DesiredCameraRotation,
	                                                 DeltaSeconds, RotationSmoothingSpeed);
	CameraBoom->SetRelativeRotation(LerpedRotation);
	SetControlRotation(DesiredCameraRotation);
}

/*
 *  Tab Targeting
 */

void ATDPController::UpdateTargetList()
{
	TargetsWithinRadius.Empty();
	
	TArray<FOverlapResult> Overlaps;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetingRadius);
	
	GetWorld()->OverlapMultiByObjectType(Overlaps, ControlledCharacter->GetActorLocation(), FQuat::Identity,FCollisionObjectQueryParams::AllDynamicObjects, CollisionShape);
	for (const FOverlapResult& Overlap : Overlaps)
		if (AActor* Actor = Overlap.GetActor(); Actor->Implements<UTargetableInterface>())
			if(TargetsWithinRadius.Find(Actor) == INDEX_NONE)
				TargetsWithinRadius.Add(Actor);

	AActor* Location = ControlledCharacter;
	TargetsWithinRadius.Sort([Location](const AActor& ActorA, const AActor& ActorB) {
		const float DistanceA = FVector::Distance(Location->GetActorLocation(), ActorA.GetActorLocation());
		const float DistanceB = FVector::Distance(Location->GetActorLocation(), ActorB.GetActorLocation());
		return DistanceA < DistanceB;
	});
}

void ATDPController::FindNewTarget()
{
	// Clear Current Target
	if(CurrentTarget)
		if(ITargetableInterface* CurrentTargetInterface = Cast<ITargetableInterface>(CurrentTarget))
			CurrentTargetInterface->TargetLost();

	// Find List Index
	int Index = CurrentTarget ? TargetsWithinRadius.Find(CurrentTarget) + 1 : 0;
	Index = Index != INDEX_NONE ? Index : 0;
	Index = Index < TargetsWithinRadius.Num() ? Index : 0;
	
	// Set New Target
	CurrentTarget = TargetsWithinRadius[Index];
	if(ITargetableInterface* CurrentTargetInterface = Cast<ITargetableInterface>(CurrentTarget))
		CurrentTargetInterface->TargetGained();
}
