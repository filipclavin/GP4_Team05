// Fill out your copyright notice in the Description page of Project Settings.


#include "LichProjectile.h"

#include "BaseEnemyClass.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALichProjectile::ALichProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_projectileCollider = CreateDefaultSubobject<USphereComponent>("collider");
	RootComponent = _projectileCollider;
}

// Called when the game starts or when spawned
void ALichProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	_projectileCollider->OnComponentBeginOverlap.AddDynamic(this, &ALichProjectile::HitPlayer);
}

// Called every frame
void ALichProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + GetActorForwardVector()*_projectileSpeed*DeltaTime, true);

	if (FVector::Distance(GetActorLocation(), _startLocation) > _projectileRange)
	{
		DespawnProjectile();
	}
}

void ALichProjectile::SpawnProjectile()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	
	_startLocation = GetActorLocation();
}

void ALichProjectile::DespawnProjectile()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ALichProjectile::HitPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool ,bool bFromSweep,const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ABaseEnemyClass>()){return;}

	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		AAuraCharacter* Player = Cast<AAuraCharacter>(OtherActor);

		Player->QueueDamage(_projectileDamage, ElementTypes::LIGHTNING);
	}

	DespawnProjectile();
}