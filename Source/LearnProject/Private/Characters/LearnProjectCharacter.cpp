// Copyright Epic Games, Inc. All Rights Reserved.

#include "LearnProject/Public/Characters/LearnProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LearnProject/Public/Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
//////////////////////////////////////////////////////////////////////////
// ALearnProjectCharacter

ALearnProjectCharacter::ALearnProjectCharacter()
{
	// 设置此字符每帧调用Tick()。如果您不需要它，可以关闭它以提高性能。
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(28.f,96.f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}


void ALearnProjectCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();
	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	Tags.Add(FName("Player"));
}

void ALearnProjectCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

bool ALearnProjectCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
	CharacterState != ECharacterState::ECS_Unequipped;
}

bool ALearnProjectCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && 
		EquippedWeapon;
}

void ALearnProjectCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ALearnProjectCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool ALearnProjectCharacter::CanAttack()
{
	return ActionState != EActionState::EAS_Attacking && CharacterState != ECharacterState::ECS_Unequipped;
}
void ALearnProjectCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

int32 ALearnProjectCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ALearnProjectCharacter::PlayAttackMontage()
{
		return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

void ALearnProjectCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage,2);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALearnProjectCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALearnProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALearnProjectCharacter::Look);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ALearnProjectCharacter::Attack);

		EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Triggered, this, &ALearnProjectCharacter::EKeyPressed);
	}
}

void ALearnProjectCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ALearnProjectCharacter::Tick(float DeltaTime)
{
}

void ALearnProjectCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALearnProjectCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void ALearnProjectCharacter::EKeyPressed(const FInputActionValue &Value)
{
	
	// if (bChangeWeaponSocket && CharacterState == ECharacterState::ECS_Unequipped)
	// {
	// 	if (EquippedWeapon)
	// 	{
	// 		EquippedWeapon->Equip(GetMesh(), FName("RightHandSocket"),this,this);
	// 		
	// 		CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
	// 	}
	// }
	// else
	// {
	// 	if (bChangeWeaponSocket && ActionState != EActionState::EAS_Attacking)
	// 	{
	// 		GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage, 1.f);
	// 		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Unequip"), EquipMontage);
	// 	}
	// 	else if (ActionState != EActionState::EAS_Attacking)
	// 	{
	// 		GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage, 1.f);
	// 		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Equip"), EquipMontage);
	// 	
	// 	}
	// 	// CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
	// }
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ALearnProjectCharacter::Attack()
{
	// if (CharacterState != ECharacterState::ECS_Unequipped && ActionState != EActionState::EAS_Attacking)
	// {
	// 	ActionState = EActionState::EAS_Attacking;
	//
	// 	int32 Number = FMath::RandRange(1, 3);
	// 	// int32 Number = 3.f;
	// 	UE_LOG(LogTemp, Warning, TEXT("%d"), Number);
	// 	FName MontageSection = FName("Default");
	// 	switch (Number)
	// 	{
	// 	case 1:
	// 		MontageSection = FName("section1");
	// 		break;
	// 	case 2:
	// 		MontageSection = FName("section2");
	// 		break;
	// 	case 3:
	// 		MontageSection = FName("section3");
	// 		break;
	// 	default:
	// 		break;
	// 	};
	// 	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 2.f);
	// 	GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageSection, AttackMontage);
	// }
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	if(CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}



