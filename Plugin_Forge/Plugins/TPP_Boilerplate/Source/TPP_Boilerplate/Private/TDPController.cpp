#include "TDPController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TargetableInterface.h"
#include "TPPCharacter.h"
#include "VectorTypes.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SpringArmComponent.h"

void ATDPController::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<ATPPCharacter>(GetCharacter()))
	{
		ControlledCharacter = Cast<ATPPCharacter>(GetCharacter());
		ControlledCharacter->SetupPlayerInputDelegate.AddDynamic(this, &ATDPController::SetupInput);

		// Camera Set Up
		CameraBoom = ControlledCharacter->GetCameraBoom();
		CameraBoom->TargetArmLength = ZoomMultiplier * ZoomStep;
		DesiredCameraRotation = CameraBoom->GetRelativeRotation();
	}
}

void ATDPController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATDPController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
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
		EnhancedInputComponent->BindAction(DropTargetAction, ETriggerEvent::Started, this, &ATDPController::ClearTarget);
	}
}

void ATDPController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Tab Targeting
	UpdateTargetList();
	UpdateRotationToTarget();
	
	// Adjust Camera
	UpdateCameraZoom(DeltaSeconds);
	UpdateCameraRotation(DeltaSeconds);
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
	if (CurrentTarget) return;
	if (!IsInputKeyDown(EKeys::MiddleMouseButton)) return;
	
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	const float HorizontalAxis = LookAxisVector.X * CurrentCameraSensitivity;
	DesiredCameraRotation.Yaw += HorizontalAxis;
}

void ATDPController::UpdateCameraZoom(const float DeltaSeconds) const
{
	if(!CameraBoom) return;
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ZoomStep * ZoomMultiplier,
													DeltaSeconds,ZoomSmoothingSpeed);
}

void ATDPController::UpdateCameraRotation(const float DeltaSeconds)
{
	if(!CameraBoom) return;

	// Get the current and desired rotations
	const FRotator CurrentRotation = CameraBoom->GetRelativeRotation();
	const FRotator DesiredRotation = DesiredCameraRotation;
	
	if (CurrentTarget)
	{
		// Calculate the rotational difference
		FRotator RotationDifference = DesiredRotation - CurrentRotation;
		RotationDifference.Normalize(); // Normalize to get the shortest path

		// Calculate the maximum rotation step we can take this frame
		FRotator MaxRotationStep = MaxRotationSpeed * DeltaSeconds * FRotator(1.0f, 1.0f, 1.0f);

		// Clamp the rotational difference to the maximum rotation step
		FRotator ClampedRotationDifference = RotationDifference;
		ClampedRotationDifference.Pitch = FMath::Clamp(RotationDifference.Pitch, -MaxRotationStep.Pitch, MaxRotationStep.Pitch);
		ClampedRotationDifference.Yaw = FMath::Clamp(RotationDifference.Yaw, -MaxRotationStep.Yaw, MaxRotationStep.Yaw);
		ClampedRotationDifference.Roll = FMath::Clamp(RotationDifference.Roll, -MaxRotationStep.Roll, MaxRotationStep.Roll);
	
		// Calculate the new rotation
		FRotator NewRotation = CurrentRotation + ClampedRotationDifference;
		
		// Set the new rotation
		CameraBoom->SetRelativeRotation(NewRotation);
		SetControlRotation(NewRotation);
	}
	else
	{
		const FRotator Lerped = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, RotationSmoothingSpeed);

		// Set the new rotation
		CameraBoom->SetRelativeRotation(Lerped);
		SetControlRotation(DesiredRotation);
	}
}

/*
 *  Tab Targeting
 */

void ATDPController::UpdateTargetList()
{
	if(!ControlledCharacter) return;
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

void ATDPController::UpdateRotationToTarget()
{
	if (!CurrentTarget) return;
	
	const FVector DirectionVector = CurrentTarget->GetActorLocation() - ControlledCharacter->GetActorLocation();
	const FRotator TargetRotation = DirectionVector.Rotation();
	
	DesiredCameraRotation.Yaw = TargetRotation.Yaw;
}

void ATDPController::FindNewTarget()
{
	if(TargetsWithinRadius.IsEmpty()) return;

	ClearTarget();
	
	// Find List Index
	int Index = CurrentTarget ? TargetsWithinRadius.Find(CurrentTarget) + 1 : 0;
	Index = Index != INDEX_NONE ? Index : 0;
	Index = Index < TargetsWithinRadius.Num() ? Index : 0;
	
	// Set New Target
	CurrentTarget = TargetsWithinRadius[Index];
	if(ITargetableInterface* CurrentTargetInterface = Cast<ITargetableInterface>(CurrentTarget))
		CurrentTargetInterface->TargetGained();
}

void ATDPController::ClearTarget()
{
	// Clear Current Target
	if(CurrentTarget)
		if(ITargetableInterface* CurrentTargetInterface = Cast<ITargetableInterface>(CurrentTarget))
			CurrentTargetInterface->TargetLost();

	CurrentTarget = nullptr;
}
