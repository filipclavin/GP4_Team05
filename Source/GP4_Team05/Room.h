// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

class UBoxComponent;
class UChaosManager;
class ALevelGenerator;
class AEnemySpawner;
class ARoomAnchor;

UCLASS()
class GP4_TEAM05_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoom();	
	virtual void BeginPlay() override;

	// Calls when players enter the Player Enter Trigger box
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnPlayerEnter();
	// Calls when room is loaded from LevelGenerator::LoadNextRoom
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnRoomLoad();
	// Called when roomed has filled chaos bar
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnRoomChaosBarFilled();
	UFUNCTION(BlueprintCallable) void OnRoomComplete();

	void ResetChaosManager();

	void OnRoomLoad_Implementation() {}
	void OnPlayerEnter_Implementation(){}
	void OnRoomChaosBarFilled_Implementation();

	UFUNCTION() void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	void ActivateRoom();
	
	void AnchorToRoom(const ARoomAnchor* anchor, const ARoom* room);
	ARoomAnchor* GetUnusedAnchor();
	UFUNCTION(BlueprintCallable) ARoomAnchor* GetOccupiedAnchor();

	virtual void Tick(float deltaTime) override;

	void SetBridgeRoom() { _bridgeRoom = true; }
protected:
	friend class ALevelGenerator;

	void FetchLevelGenerator();

	float _testDuration = 2.0f;
	// Int to check which anchor we came from
	INT32 _occupiedAnchor = -1;
	bool _bridgeRoom      = false;
	bool _hasEntered      = false;
	bool _roomIsCompleted = false;
	
	UChaosManager* _chaosManager;

	// -1 = it will not pick a specific entrance. 0 - 3 to specify what door to use.
	//UPROPERTY(BlueprintReadWrite) bool				    _roomCompleted = false;
	UPROPERTY(EditAnywhere)	      int				   _prioritizeEntrance = -1;
	UPROPERTY(BlueprintReadOnly)  int		           _roomDepth = -1;
								  bool			       _colliderActiveOnSpawn = false;
	UPROPERTY(EditAnywhere)       AEnemySpawner*	   _enemySpawner;
	UPROPERTY(EditAnywhere)       TArray<ARoomAnchor*> _anchors;
	UPROPERTY(EditAnywhere)       UBoxComponent*       _playerEnterTrigger;
	UPROPERTY(BlueprintReadWrite) ALevelGenerator*     _levelGenerator = nullptr;
};
