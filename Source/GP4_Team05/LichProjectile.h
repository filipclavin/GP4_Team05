// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LichProjectile.generated.h"

class USphereComponent;

UCLASS()
class GP4_TEAM05_API ALichProjectile : public AActor
{
	GENERATED_BODY()
	
	FVector _startLocation;

	
	
	
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _projectileSpeed	= 5000;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _projectileRange	= 50000;
	UPROPERTY(EditDefaultsOnly, Category="Projectile Stats") int _projectileDamage	= 10;

	 
public:	
	// Sets default values for this actor's properties
	ALichProjectile();

	UFUNCTION(BlueprintCallable) virtual void SpawnProjectile  ();
	virtual void DespawnProjectile();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
UFUNCTION()
	virtual void HitPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool ,bool bFromSweep,const FHitResult& SweepResult);

	
	UPROPERTY(VisibleAnywhere) USphereComponent* _projectileCollider = nullptr;
};
