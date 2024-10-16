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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* _collisionBox;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* _puddleMesh;
	// Dynamic material to control fading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UMaterialInstanceDynamic* _dynamicMaterial;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fade")
	float _fadeSpeedStart;  // Fade speed at the start
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fade")
	float _fadeSpeedEnd;  // Faster fade speed towards the end
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fade")
	float _shrinkSpeed;  // Shrink speed over time

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fade") float _fadeSpeed;
		
	// Percent of total health this puddle heals (e.g., 30% of max health)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing")
	float _healingPercentage;
	// Healing rate applied over time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing")
	float _healingRate;	
	float _healingProgress;
	//float _totalHealed;
	float _opacity;	
	bool  _bPlayerOnPuddle;

	UPROPERTY()
	class AAuraCharacter* _auraCharacter;
	class AAuraHandler*	  _auraHandler;

	//this is just for a temporary implementation of adding chaos, CHANGE LATER! -Gustav
	UPROPERTY()
	      UChaosManager*  _chaosManager;
		 /* UPROPERTY(EditDefaultsOnly, Category = "Blood Puddle")
		  TSubclassOf<ABloodPuddle> BloodPuddleBlueprint;
		  */
	// Handle player stepping on the puddle
	UFUNCTION()
	void OnPlayerEnterPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void FadeOut(float DeltaTime);
	void ApplyHealing(float DeltaTime);
	void ShrinkPuddle(float DeltaTime);
	
public:	
	
	virtual void Tick(float DeltaTime) override;
	//UFUNCTION(BlueprintCallable, Category = "BloodPuddle")
	//static ABloodPuddle* SpawnPuddle(FVector SpawnLocation, FRotator SpawnRotation);

};
