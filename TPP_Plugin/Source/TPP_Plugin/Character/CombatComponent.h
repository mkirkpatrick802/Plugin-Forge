#pragma once

#include "CoreMinimal.h"
#include "TPPHUD.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000

class ATPPHUD;
class ATPPController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPP_PLUGIN_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	friend class ATPPCharacter;

	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(class AWeapon* WeaponToEquip);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void FireButtonPressed(bool Pressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void SetHUDCrosshairs(float DeltaTime);

private:

	void ToggleAiming();

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool Aiming);

	void InterpFOV(float DeltaTime);

	void StartFireTimer();
	void FireTimerFinished();


private:

	UPROPERTY()
	ATPPCharacter* Character;

	UPROPERTY()
	ATPPController* Controller;

	UPROPERTY()
	ATPPHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool IsAiming;

	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool IsFireButtonPressed;

	FHUDPackage HUDPackage;
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;
	bool CanFire = true;

	/*
	 *	Aiming and FOV
	*/

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	float CurrentFOV;

	/*
	 *	Automatic Fire
	*/

	FTimerHandle FireTimer;
};
