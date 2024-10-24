// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GP4AIController.generated.h"

/**
 * 
 */
UCLASS()
class GP4_TEAM05_API AGP4AIController : public AAIController
{
	GENERATED_BODY()

public:
	AGP4AIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(EditAnywhere) float							separationWeight			= 50.0f;
	UPROPERTY(EditAnywhere) float							avoidanceRangeMultiplier	= 1.0f;
	
	UPROPERTY() class UCrowdFollowingComponent* CrowdFollowingComponent;
	
	virtual void BeginPlay() override;
};
