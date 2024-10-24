// Fill out your copyright notice in the Description page of Project Settings.


#include "GP4AIController.h"

#include "Navigation/CrowdFollowingComponent.h"

AGP4AIController::AGP4AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	
}

void AGP4AIController::BeginPlay()
{
	Super::BeginPlay();

	CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();
	if (!CrowdFollowingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CrowdFollowingComponent not found in AGP4AIController"));
		return;
	}
	
	CrowdFollowingComponent->SetCrowdSeparation(true);
	CrowdFollowingComponent->SetCrowdSeparationWeight(separationWeight);
	CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(avoidanceRangeMultiplier);
	CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
}
