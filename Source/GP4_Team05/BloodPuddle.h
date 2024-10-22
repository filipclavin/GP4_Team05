// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloodPuddle.generated.h"

class UChaosManager;
class AAuraHandler;

UCLASS()
class GP4_TEAM05_API ABloodPuddle : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABloodPuddle();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puddle")
	class UBoxComponent* _collisionBox;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puddle")
	class UStaticMeshComponent* _puddleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puddle")
	class UMaterialInstanceDynamic* _dynamicMaterial;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puddle")
	float _fadeSpeedStart; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puddle")
	float _fadeSpeedEnd; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puddle")
	float _shrinkSpeed;  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puddle")
	int32 _healingPercent;

	float _opacity;	
	bool  _bPlayerOnPuddle;
	float _puddleTickDuration;  
	const float _tickInterval = 1.0f;;

	UPROPERTY()
	class AAuraCharacter* _auraCharacter;
	class AAuraHandler*	  _auraHandler;

	//this is just for a temporary implementation of adding chaos, CHANGE LATER! -Gustav
	UPROPERTY()
	      UChaosManager*  _chaosManager;		 
	UFUNCTION()
	void OnPlayerEnterPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerExitPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintImplementableEvent, Category = "Puddle")
	void OnPlayerAbsorbingBlood();



	void FadeOut(float DeltaTime);
	void ApplyHealing(float DeltaTime);
	void ShrinkPuddle(float DeltaTime);
	
public:	
	
	virtual void Tick(float DeltaTime) override;
	

};
