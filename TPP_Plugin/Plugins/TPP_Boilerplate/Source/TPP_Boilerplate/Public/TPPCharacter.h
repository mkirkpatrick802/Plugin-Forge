#pragma once

#include "CoreMinimal.h"
#include "InteractWithCrosshairsInterface.h"
#include "GameFramework/Character.h"
#include "TurningInPlace.h"
#include "TPPCharacter.generated.h"

class AWeapon;
struct FInputActionValue;

UCLASS()
class TPP_BOILERPLATE_API ATPPCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{

	GENERATED_BODY()

	/** Components */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	class UCombatComponent* Combat;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;


public:

	/**
	 *	Public Function
	 */

	ATPPCharacter();

	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_ReplicatedMovement() override;

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	AWeapon* GetEquippedWeapon();
	bool IsAiming();

	void PlayFireMontage(bool IsAiming);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();

protected:

	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for crouch input */
	void ToggleCrouch(const FInputActionValue& Value);

	/** Called for interact input */
	void Interact(const FInputActionValue& Value);

	/** Called for aim input */
	void ToggleAim(const FInputActionValue& Value);

	void CalculateAO_Pitch();
	void AimOffset(float DeltaTime);
	void SimProxiesTurn();

	virtual void Jump() override;

	void StartFiringWeapon(const FInputActionValue& Value);
	void StopFiringWeapon(const FInputActionValue& Value);

	void PlayHitReactMontage();

private:

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerInteractButtonPressed();

	void TurnInPlace(float DeltaTime);
	void HideCameraIfCharacterClose();
	float CalculateSpeed();

private:

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;

	bool RotateRootBone;
	float TurnThreshold = .5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

public:

	/**
	*	Getters & Setters
	*/

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return RotateRootBone; }
	FVector GetHitTarget() const;
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};