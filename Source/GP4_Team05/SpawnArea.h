// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SpawnArea.generated.h"

UCLASS()
class GP4_TEAM05_API ASpawnArea : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) UBoxComponent* _boxComponent;
	
public:
	FBoxSphereBounds GetBounds() const { return _boxComponent->Bounds; }
	
	// Sets default values for this actor's properties
	ASpawnArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
