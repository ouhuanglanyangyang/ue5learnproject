// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "LearnProject/Public/Components/AttributeComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Attribute = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::PlayHitReactMontage(FName SectionName)
{
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return 0;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	UE_LOG(LogTemp, Display, TEXT("GetHit"));
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLosered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLosered - GetActorLocation()).GetSafeNormal();
	double Theta = FMath::Acos(FVector::DotProduct(Forward, ToHit));
	Theta = FMath::RadiansToDegrees(Theta);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
		Theta *= -1.f;
	FName SectionName = FName("");
	if (Theta >= -45.f && Theta < 45.f)
		SectionName = FName("FromFront");
	if (Theta >= 45.f && Theta < 135.f)
		SectionName = FName("FromRight");
	if (Theta >= -135.f && Theta < -45.f)
		SectionName = FName("FromLeft");
	if (Theta >= 135.f && Theta < -135.f)
		SectionName = FName("FromBack");
	PlayHitReactMontage(SectionName);
}

// void ABaseCharacter::Attack()
// {
// 	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
// 	{
// 		CombatTarget = nullptr;
// 	}
// }

// bool ABaseCharacter::CanAttack()
// {
// 	return false;
// }


void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		UE_LOG(LogTemp, Warning, TEXT("Set Weapon Collision Enabled Or Unabled"));
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

