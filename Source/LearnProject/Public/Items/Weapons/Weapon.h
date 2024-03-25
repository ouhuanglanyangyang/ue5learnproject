// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LearnProject/Public/Items/Item.h"
// #include "Components/SphereComponent.h"
// #include "LearnProject/Public/Enemy/Enemy.h"
#include "Weapon.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ALearnProjectCharacter;
class UBoxComponent;

UCLASS()
class LEARNPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere)
	float Damage = 20.0f;
public:	
	// Sets default values for this actor's properties
	AWeapon();
	// UPROPERTY(VisibleAnywhere)
	// USphereComponent* Sphere;
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	// UStaticMeshComponent* Item;
	UFUNCTION()
	void Equip(USceneComponent *InParent,FName InSocketName,AActor* NewOwner,APawn* NewInstigator);

	UFUNCTION()
	FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox;}

	TArray<AActor*> IgnoreActors;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// UFUNCTION()
	// void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult&  SweepResult);
	// UFUNCTION()
	// void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
