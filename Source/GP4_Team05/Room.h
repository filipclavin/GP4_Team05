// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

class UBoxComponent;
class ALevelGenerator;
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
	UFUNCTION(BlueprintCallable) void OnRoomComplete();

	void OnRoomLoad_Implementation() {}
	void OnPlayerEnter_Implementation(){}

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
		
	UPROPERTY(BlueprintReadOnly)  int		           _roomDepth = -1;
	UPROPERTY(EditAnywhere)       bool			       _colliderActiveOnSpawn = false;
	UPROPERTY(EditAnywhere)       TArray<ARoomAnchor*> _anchors;
	UPROPERTY(EditAnywhere)       UBoxComponent*       _playerEnterTrigger;
	UPROPERTY(BlueprintReadWrite) ALevelGenerator*     _levelGenerator = nullptr;
};
