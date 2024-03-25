// Fill out your copyright notice in the Description page of Project Settings.

#include "LearnProject/Public/Items/Weapons/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "LearnProject/Public/Characters/LearnProjectCharacter.h"
#include "LearnProject/Public/Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	// RootComponent = Item;
	// Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	// Sphere->SetupAttachment(GetRootComponent());
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::Equip(USceneComponent *InParent, FName InSocketName, AActor *NewOwner, APawn *NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	// Item->AttachToComponent(InParent, TransformRules, InSocketName);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	// Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	// Sphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

// void AWeapon::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
// {
// 	ALearnProjectCharacter *LearnProjectCharacter = Cast<ALearnProjectCharacter>(OtherActor);
// 	if (LearnProjectCharacter)
// 	{
// 		LearnProjectCharacter->SetOverlappingWeapon(this);
// 	}
// }
//
// void AWeapon::OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
// {
// 	ALearnProjectCharacter *LearnProjectCharacter = Cast<ALearnProjectCharacter>(OtherActor);
// 	if (LearnProjectCharacter && Sphere->IsCollisionEnabled())
// 	{
// 		LearnProjectCharacter->SetOverlappingWeapon(nullptr);
// 	}
// }

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Box Trace"));
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	TArray<AActor *> ActorToIgnore;
	ActorToIgnore.Add(this);
	for (AActor *Actor : IgnoreActors)
	{
		ActorToIgnore.AddUnique(Actor);
	}
	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(16, 16, 16),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorToIgnore,
		EDrawDebugTrace::ForDuration,
		// EDrawDebugTrace::None,
		BoxHit,
		true
		);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (BoxHit.GetActor() && BoxHit.GetActor() != this)
	{
		UE_LOG(LogTemp, Display, TEXT("Hit"));
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass());
		IHitInterface *HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			FVector ImpactPoint = BoxHit.ImpactPoint;
			IHitInterface::Execute_GetHit(BoxHit.GetActor(), ImpactPoint);
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());
		CreateFields(BoxHit.ImpactPoint);
	}
}
