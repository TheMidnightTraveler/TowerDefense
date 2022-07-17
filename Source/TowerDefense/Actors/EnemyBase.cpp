// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "TrackLine.h"
#include "Components/SplineComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	//Mesh->SetupAttachment(RootComponent);

	MovementSpeed = 100.f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (AlphaCurveFloat && TrackLine)
    {
		Path = TrackLine->PathLine;
		float Seconds = Path->GetSplineLength()/MovementSpeed;
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Seconds to walk all path = %f"), Seconds));

        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("MoveAlongTheLine"));

        MoveTimeline.AddInterpFloat(AlphaCurveFloat, ProgressFunction);
		MoveTimeline.SetPlayRate(1.f/Seconds);
        MoveTimeline.PlayFromStart();
    }
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTimeline.TickTimeline(DeltaTime);
}

void AEnemyBase::MoveAlongTheLine(float Value)
{
	float Distance = FMath::Lerp(0.f, Path->GetSplineLength(), Value);
	FVector LocationAlongSpline = Path->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Type::World);
	FRotator RotationAlongSpline = Path->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Type::World);

	SetActorLocationAndRotation(LocationAlongSpline, RotationAlongSpline);
}
