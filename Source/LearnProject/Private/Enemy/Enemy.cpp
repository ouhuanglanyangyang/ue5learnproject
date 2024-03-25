// 在项目设置的Description页面中填写版权信息

#include "LearnProject/Public/Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "LearnProject/Public/Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LearnProject/Public/HUD/HealthBarComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
// #include "DebugMacros.h"

// 设置默认值
AEnemy::AEnemy()
{
    // 设置此字符每帧调用Tick()。如果您不需要它，可以关闭它以提高性能。
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);


    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());
    HealthBarWidget->SetVisibility(false);

    PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    PawnSensing->SightRadius = 4000.f;
    PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Die()
{
    int32 Number = FMath::RandRange(1, 5);
    FName MontageSection = FName("Default");
    switch (Number)
    {
    case 1:
        MontageSection = FName("Death1");
        DeathPose = EDeathPose::EDP_Death1;
        break;
    case 2:
        MontageSection = FName("Death2");
        DeathPose = EDeathPose::EDP_Death2;

        break;
    case 3:
        MontageSection = FName("Death3");
        DeathPose = EDeathPose::EDP_Death3;

        break;
    case 4:
        MontageSection = FName("Death4");
        DeathPose = EDeathPose::EDP_Death4;

        break;
    case 5:
        MontageSection = FName("Death5");
        DeathPose = EDeathPose::EDP_Death5;

        break;
    default:
        break;
    };
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->DisableMovement();
    GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage, 1.f);
    GetMesh()->GetAnimInstance()->Montage_JumpToSection(MontageSection, DeathMontage);
    HealthBarWidget->SetVisibility(false);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(8.f);
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint)
{
    if (Attribute && Attribute->IsAlive())
    {
        DirectionalHitReact(ImpactPoint);

        // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Theta: %f"), Theta));
        // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100, 10.0f, FColor::Red, 5.f);
        // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100, 10.0f, FColor::Green, 5.f);
        // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100, 10.0f, FColor::Blue, 5.f);
    }
    else
    {
        Die();
    }
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    if (Attribute && HealthBarWidget)
    {
        Attribute->ReceiveDamage(DamageAmount);
        HealthBarWidget->SetHealthPercent(Attribute->GetHealthPercent());
        UE_LOG(LogTemp, Display, TEXT("HealthPercent: %f"), Attribute->GetHealthPercent());
    }
    // EnemyController = Cast<AAIController>(EventInstigator);
    CombatTarget = EventInstigator->GetPawn();
    EnemyState = EEnemyState::EES_Chasing;
    MoveToTarget(CombatTarget);
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
    return DamageAmount;
}

void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}

// 游戏开始时或当被孵化时调用
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);
    HealthBarWidget->SetVisibility(false);

    PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::SeenPawn);

    UWorld *World = GetWorld();
    if(World && WeaponClass)
    {
           AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass);
            Weapon->Equip(GetMesh(),FName("RightHandSocket"),this,this);
    }
}

bool AEnemy::InTargetRange(AActor *Target, double Radius)
{
    if (Target == nullptr)
        return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    // DRAW_SPHERE_SingleFrame(GetActorLocation());
    // DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
    return DistanceToTarget <= Radius;
}
void AEnemy::MoveToTarget(AActor *Target)
{
    if (EnemyController == nullptr || Target == nullptr)
        return;
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(32.f);
    EnemyController->MoveTo(MoveRequest);
}
AActor *AEnemy::ChoosePatrolTarget()
{

    TArray<AActor *> ValidTargets;
    for (auto Target : PatrolTargets)
    {
        if (Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }
    const int32 NumPatrolTargets = ValidTargets.Num();
    if (NumPatrolTargets)
    {
        const int32 TargetSection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSection];
    }
    return nullptr;
}
void AEnemy::SeenPawn(APawn *Pawn)
{
    if (EnemyState == EEnemyState::EES_Chasing)
        return;
    if (Pawn->ActorHasTag(FName("Player")))
    {

        GetWorldTimerManager().ClearTimer(PatrolTimer);
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        CombatTarget = Pawn;
        if (EnemyState != EEnemyState::EES_Attacking)
        {
            EnemyState = EEnemyState::EES_Chasing;
            MoveToTarget(CombatTarget);
        }
    }
}
// 每帧调用
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }
}

void AEnemy::CheckPatrolTarget()
{
    if (InTargetRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        double Random = FMath::RandRange(5.f, 10.f);
        HealthBarWidget->SetVisibility(false);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, Random, false);
    }
}

void AEnemy::CheckCombatTarget()
{
    if (!InTargetRange(CombatTarget, CombatRadius))
    {
        CombatTarget = nullptr;
        if (HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(false);
        }
        EnemyState = EEnemyState::EES_Patrolling;
        GetCharacterMovement()->MaxWalkSpeed = 150.f;
        MoveToTarget(PatrolTarget);
        UE_LOG(LogTemp, Warning, TEXT("Enemy Patrolling"));
    }
    // else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
    else if (!InTargetRange(CombatTarget, AttackRadius))
    {
        EnemyState = EEnemyState::EES_Chasing;
        if (HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(true);
        }
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        MoveToTarget(CombatTarget);
        UE_LOG(LogTemp, Warning, TEXT("Enemy Chasing"));
    }
    // else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
    else if (InTargetRange(CombatTarget, AttackRadius))
    {
        EnemyState = EEnemyState::EES_Attacking;
        if (HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(true);
        }
        UE_LOG(LogTemp, Warning, TEXT("Enemy Attacking"));
    }
}

// 用于绑定功能到输入
void AEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
