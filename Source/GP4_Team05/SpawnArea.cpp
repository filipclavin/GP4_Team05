// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnArea.h"

// Sets default values
ASpawnArea::ASpawnArea()
{
 	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_boxComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	_boxComponent->SetBoxExtent(FVector(100, 100, 10));
	_boxComponent->SetHiddenInGame(true);
	_boxComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawnArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

