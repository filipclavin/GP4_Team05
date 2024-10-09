// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBaseClass.generated.h"

class USphereComponent;

UCLASS()
class GP4_TEAM05_API AProjectileBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBaseClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere) USphereComponent* _projectileCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _baseProjectileSpeed		 = 5000;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _baseProjectileRange		 = 50000;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _baseProjectileForking		 = 0;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _baseProjectileForkingRange = 100;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _baseProjectileDamage		 = 20;
	
	int _projectileSpeed   = 5000;
	int _projectileForking = 0;
	int _projectileDamage  = 20;
	int _projectileRange   = 50000;

	FVector _startLocation;

	void SpawnProjectile  (int additionalSpeed, int additionalRange, int additionalForkAmount, int additionalDamage);
	void DespawnProjectile();

	UFUNCTION()
	void HitEnemies(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool ,bool bFromSweep,const FHitResult& SweepResult);
};
