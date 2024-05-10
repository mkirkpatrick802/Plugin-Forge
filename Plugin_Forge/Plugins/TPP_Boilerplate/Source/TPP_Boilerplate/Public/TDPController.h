// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "TPPController.h"
#include "TDPController.generated.h"

class ATPPCharacter;

UCLASS()
class TPP_BOILERPLATE_API ATDPController : public ATPPController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* ControllerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetAction;

public:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void SetupInput();

	virtual void Tick(float DeltaSeconds) override;

private:

	void Zoom(const FInputActionValue& InputActionValue);

private:

	UPROPERTY()
	ATPPCharacter* ControlledCharacter;
	
};
