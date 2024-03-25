// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "LearnProject/Public/Characters/CharacterTypes.h"
#include "Enemy.generated.h"
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
UCLASS()
class LEARNPROJECT_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual void Die() override;


private:


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent *HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent *PawnSensing;





	UPROPERTY()
	AActor *CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY()
	AAIController *EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor *PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor *> PatrolTargets;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	bool InTargetRange(AActor *Target, double Radius);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	void MoveToTarget(AActor *Target);

	AActor *ChoosePatrolTarget();

	UFUNCTION(BlueprintCallable)
	void SeenPawn(APawn* Pawn);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector &ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser);
};
