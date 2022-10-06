// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerBase.h"
#include "TowerDefense/macros.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATowerBase::ATowerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TowerSceneComponent"));
	}

	ShootRange = CreateDefaultSubobject<USphereComponent>("Shoot Range");
	ShootRange->SetupAttachment(RootComponent);
	ShootRange->OnComponentBeginOverlap.AddDynamic(this, &ATowerBase::OnEnemyEnterShootRange);
	OnActorEndOverlap.AddDynamic(this, &ATowerBase::OnEnemyOutOfShootRangeOrDied);

	TowerBase = CreateDefaultSubobject<UStaticMeshComponent>("Tower Base");
	TowerBase->SetupAttachment(ShootRange);
	TowerBase->SetGenerateOverlapEvents(false);

	TowerTop = CreateDefaultSubobject<UStaticMeshComponent>("Tower Top");
	TowerTop->SetupAttachment(TowerBase);
	TowerTop->SetGenerateOverlapEvents(false);

	ShootLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(TowerTop);
	
	ShootRate = 1.f;

	
}

// Called when the game starts or when spawned
void ATowerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATowerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATowerBase::Shoot()
{
	D("Shoot!");
	GetWorldTimerManager().SetTimer(ShootTimeHandle, this, &ATowerBase::ShootHandle, ShootRate, true);
}

void ATowerBase::ShootHandle()
{
	if (!ShootTarget->FindComponentByClass<UCapsuleComponent>()) return;
	FVector Loc = ShootTarget->FindComponentByClass<UCapsuleComponent>()->GetComponentLocation();
	DEBUGMESSAGE("Collision location = %s", *Loc.ToString());
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(TowerTop->GetComponentLocation(), Loc);
	NewRotation = FRotator(0, NewRotation.Yaw, 0);
	TowerTop->SetWorldRotation(NewRotation);
}

void ATowerBase::FindTarget()
{
	if (!EnemyClass) return;

	TArray<AActor*> TargetsInRange;
	ShootRange->GetOverlappingActors(TargetsInRange, EnemyClass);
	if (!TargetsInRange.IsEmpty())
	{
		ShootTarget = TargetsInRange[0];
		Shoot();
		DEBUGMESSAGE("%s - Shoot Target", *(ShootTarget->GetName()));
	}
	else
	{
		D("There is no enemy in shoot range");
		GetWorldTimerManager().ClearTimer(ShootTimeHandle);
	} 
}

void ATowerBase::OnEnemyEnterShootRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!Cast<AEnemyBase>(OtherActor)) return;
	DEBUGMESSAGE("%s - in shoot range", *(OtherActor->GetName()));
	if(!IsValid(ShootTarget)) FindTarget();
}

void ATowerBase::OnEnemyOutOfShootRangeOrDied(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Cast<AEnemyBase>(OtherActor)) return;
	DEBUGMESSAGE("%s - is out of range or died", *(OtherActor->GetName()));
	if(IsValid(ShootTarget) && ShootTarget == OtherActor) FindTarget();
}