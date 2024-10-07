// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraHandler.generated.h"

class UAura;
class AAuraCharacter;

UCLASS()
class GP4_TEAM05_API AAuraHandler : public AActor
{
	GENERATED_BODY()
public:	
	AAuraHandler();

	UFUNCTION(BlueprintCallable) int     GetAuraIDbyName(FString name);
	UFUNCTION(BlueprintCallable) FString GetAuraNameByID(int id);

	UFUNCTION(BlueprintCallable) void CastAuraByID(const int id, AAuraCharacter* target, AAuraCharacter* caster);
	UFUNCTION(BlueprintCallable) void CastAuraByName(FString name, AAuraCharacter* target, AAuraCharacter* caster);

protected:
	UFUNCTION(BlueprintCallable) void FetchAllAurasAttached();
	
	UPROPERTY(BlueprintReadOnly) 
	TArray<UAura*>       _auraList;
	TMap<FString, INT32> _auraNameMap; 
	
	// The AActor that all the pooled Auras are attached to!
	UPROPERTY(BlueprintReadWrite, EditAnywhere) AActor* _auraPoolingActor  = nullptr;
	UPROPERTY(EditAnywhere)					    int     _minimunPoolCount  = 20; 

private:
	UAura* GetPooledAura(UAura* aura);

	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
};
