// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LearnProject/Public/Interfaces/HitInterface.h"
#include "LearnProject/Public/Items/Weapons/Weapon.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;

UCLASS()
class LEARNPROJECT_API ABaseCharacter : public ACharacter,public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual void Die();
	virtual  void PlayHitReactMontage(FName SectionName);
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual int32 PlayAttackMontage();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	UPROPERTY(VisibleAnywhere)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere)
	UAnimMontage *HitReactMontage;
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent *Attribute;
	UPROPERTY(EditAnywhere)
	UAnimMontage *DeathMontage;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
};

