#include "AudioManager.h"
#include "Components/AudioComponent.h"

UAudioManager::UAudioManager()
{
   
}

void UAudioManager::PlaySoundAtLocation(UWorld* World, FName SoundName, FVector Location, float Volume, float Pitch)
{
    USoundBase* Sound = GetSoundByName(SoundName);
    if (Sound && World)
    {
        UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, Volume, Pitch);
    }
}

UAudioComponent* UAudioManager::PlaySoundAttached(FName SoundName, USceneComponent* Component, float Volume, float Pitch)
{
    USoundBase* Sound = GetSoundByName(SoundName);
    if (Sound && Component)
    {
        return UGameplayStatics::SpawnSoundAttached(Sound, Component, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, Volume, Pitch);
    }
    return nullptr;
}

void UAudioManager::StopSound(UAudioComponent* AudioComponent)
{
    if (AudioComponent)
    {
        AudioComponent->Stop();
    }
}

void UAudioManager::ModifySoundVolume(UAudioComponent* AudioComponent, float NewVolume)
{
    if (AudioComponent)
    {
        AudioComponent->SetVolumeMultiplier(NewVolume);
    }
}

USoundBase* UAudioManager::GetSoundByName(FName SoundName) const
{
    for (const FSoundEntry& Entry : SoundList)
    {
        if (Entry.SoundName == SoundName)
        {
            return Entry.Sound;
        }
    }
    return nullptr;
}

