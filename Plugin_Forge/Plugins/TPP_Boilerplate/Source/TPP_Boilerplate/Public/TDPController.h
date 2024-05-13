#pragma once
#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "TPPController.h"
#include "TDPController.generated.h"

class USpringArmComponent;
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
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetAction;

public:

	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void SetupInput();

	virtual void Tick(float DeltaSeconds) override;

private:
	void Zoom(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	
	void UpdateCameraZoom(float DeltaSeconds) const;
	void UpdateCameraRotation(float DeltaSeconds);

	void FindNewTarget();

	void UpdateTargetList();

private:

	UPROPERTY()
	ATPPCharacter* ControlledCharacter;

	UPROPERTY()
	USpringArmComponent* CameraBoom;
	
	/*
	 *		Camera Settings
	 */

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ZoomStep = 500;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ZoomSmoothingSpeed = 3;
	
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ZoomMultiplier = 2;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	FVector2D ZoomBounds = FVector2D(1, 3);

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float RotationAmount = 45;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float RotationSmoothingSpeed = 3;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CurrentCameraSensitivity = 1;
	
	FRotator DesiredCameraRotation;

	/*
	 *		Tab Targeting
	 */

	UPROPERTY(EditAnywhere, Category = "Tab Targeting")
	float TargetingRadius = 100;
	
	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY()
	TArray<AActor*> TargetsWithinRadius;
	
};
