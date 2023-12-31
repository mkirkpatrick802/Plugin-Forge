#include "TPPCharacter.h"

#include "CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TPP_Plugin/TPP_Plugin.h"
#include "TPP_Plugin/Weapon/Weapon.h"

ATPPCharacter::ATPPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 0, 600);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Combat->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ATPPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}

void ATPPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPPCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ATPPCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATPPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATPPCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPPCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPPCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATPPCharacter::Interact);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATPPCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATPPCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &ATPPCharacter::ToggleCrouch);

		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATPPCharacter::ToggleAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATPPCharacter::ToggleAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ATPPCharacter::ToggleAim);

		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATPPCharacter::StartFiringWeapon);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATPPCharacter::StopFiringWeapon);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ATPPCharacter::StopFiringWeapon);
	}

}

void ATPPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if(TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}

		CalculateAO_Pitch();
	}

	HideCameraIfCharacterClose();
}

void ATPPCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ATPPCharacter::HideCameraIfCharacterClose()
{
	if(!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float ATPPCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return Velocity.Length();
}

/*
 *	Character Movement
 */

void ATPPCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATPPCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATPPCharacter::ToggleCrouch(const FInputActionValue& Value)
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ATPPCharacter::Jump()
{
	/*if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{*/
		Super::Jump();
	//}
}

/*
 *	Object Interaction
 */

void ATPPCharacter::Interact(const FInputActionValue& Value)
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerInteractButtonPressed();
		}
	}
}

void ATPPCharacter::ServerInteractButtonPressed_Implementation()
{
	Combat->EquipWeapon(OverlappingWeapon);
}

/*
 *	Weapon Logic
 */

void ATPPCharacter::StartFiringWeapon(const FInputActionValue& Value)
{
	if(!Combat) return;
	Combat->FireButtonPressed(true);
}

void ATPPCharacter::StopFiringWeapon(const FInputActionValue& Value)
{
	if (!Combat) return;
	Combat->FireButtonPressed(false);
}

void ATPPCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ATPPCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}


bool ATPPCharacter::IsWeaponEquipped()
{
	return Combat && Combat->EquippedWeapon;
}

AWeapon* ATPPCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void ATPPCharacter::ToggleAim(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->ToggleAiming();
	}
}

bool ATPPCharacter::IsAiming()
{
	return Combat && Combat->IsAiming;
}

void ATPPCharacter::PlayFireMontage(bool IsAiming)
{
	if(!Combat) return;
	if(!Combat->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		const FName SectionName = IsAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ATPPCharacter::PlayHitReactMontage()
{
	if (!Combat) return;
	if (!Combat->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		const FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ATPPCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}

void ATPPCharacter::AimOffset(float DeltaTime)
{
	if(Combat && !Combat->EquippedWeapon) return;

	const float Speed = CalculateSpeed();
	const bool IsinAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0 && !IsinAir)
	{
		RotateRootBone = true;
		const FRotator CurrentAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
			InterpAO_Yaw = AO_Yaw;

		bUseControllerRotationYaw = true;
		CameraBoom->bEnableCameraRotationLag = false;
		TurnInPlace(DeltaTime);
	}

	if(Speed > 0 || IsinAir)
	{
		RotateRootBone = false;
		StartingAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		AO_Yaw = 0;

		bUseControllerRotationYaw = true;
		CameraBoom->bEnableCameraRotationLag = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ATPPCharacter::SimProxiesTurn()
{
	if (!Combat || !Combat->EquippedWeapon) return;

	RotateRootBone = false;

	const float Speed = CalculateSpeed();
	if(Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if(FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if(ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}

		return;
	}

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ATPPCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// Map pitch from [270, 360) to [-90, 0)
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ATPPCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0, DeltaTime, 4.f);

		AO_Yaw = InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		}
	}
}

FVector ATPPCharacter::GetHitTarget() const
{
	if (!Combat) return FVector();
	return Combat->HitTarget;
}
