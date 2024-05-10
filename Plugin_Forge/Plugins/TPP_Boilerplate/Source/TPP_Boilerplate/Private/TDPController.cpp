// Fill out your copyright notice in the Description page of Project Settings.


#include "TDPController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TPPCharacter.h"

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
	}
}

void ATDPController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATDPController::Zoom(const FInputActionValue& InputActionValue)
{
	const float ZoomDelta = InputActionValue.Get<float>();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Zoom: %f"), ZoomDelta));
}
