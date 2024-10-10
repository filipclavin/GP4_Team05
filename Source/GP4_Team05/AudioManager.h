// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "AudioManager.generated.h"

USTRUCT(BlueprintType)
struct FSoundEntry
{
    GENERATED_BODY()

    // Name for identifying the sound
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    FName SoundName;

    // The actual sound asset (e.g., USoundCue or USoundWave)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* Sound;
};

UCLASS(Blueprintable)
class GP4_TEAM05_API UAudioManager : public UObject
{
	GENERATED_BODY()
	
public:
    // Constructor
    UAudioManager();

    // Play sound at location
    UFUNCTION(BlueprintCallable, Category = "Sound")
    void PlaySoundAtLocation(UWorld* World, FName SoundName, FVector Location, float Volume = 1.0f, float Pitch = 1.0f);

    // Play sound attached to a component
    UFUNCTION(BlueprintCallable, Category = "Sound")
    UAudioComponent* PlaySoundAttached(FName SoundName, USceneComponent* Component, float Volume = 1.0f, float Pitch = 1.0f);

    // Stop a specific sound
    UFUNCTION(BlueprintCallable, Category = "Sound")
    void StopSound(UAudioComponent* AudioComponent);

    // Modify the volume of a specific sound
    UFUNCTION(BlueprintCallable, Category = "Sound")
    void ModifySoundVolume(UAudioComponent* AudioComponent, float NewVolume);

    // Find sound by name
    UFUNCTION(BlueprintCallable, Category = "Sound")
    USoundBase* GetSoundByName(FName SoundName) const;

    // Sound list that can be modified in the editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TArray<FSoundEntry> SoundList;
};
