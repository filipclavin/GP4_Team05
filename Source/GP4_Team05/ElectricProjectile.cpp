#include "ElectricProjectile.h"

#include "AuraCharacter.h"
#include "BloodPuddle.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AElectricProjectile::SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer)
{
	Super::SpawnProjectile(upgradeAmount, owningPlayer);

	if (upgradeAmount >= enemyTypeFilterThreshold)
	{
		_filterEnemyType = true;
	}
	if (upgradeAmount >= enemyHitOverspillThreshold)
	{
		_overspillHits = true;
	}
}


void AElectricProjectile::HitEnemies(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent*OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)){return;}

	if (!OtherActor->IsA<AAuraCharacter>()) {DespawnProjectile();return;}
	
	TArray<AActor*>	lightningHits;
	TArray<AActor*>	puddlesHits;
	TArray<AActor*>	alreadyHitActors;
	TQueue<AActor*> ActorsToProcess;

	UClass* AuraCharacterClass;
	_electricityForks.Empty();
	

	if (!_filterEnemyType)
	{
		AuraCharacterClass = AAuraCharacter::StaticClass();
	}
	else
	{
		AuraCharacterClass = OtherActor->GetClass();
	}
	UClass* PuddleClass		   = ABloodPuddle::StaticClass();
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<TEnumAsByte<EObjectTypeQuery>> puddleObjectType;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	
	AAuraCharacter* hitCharacter = Cast<AAuraCharacter>(OtherActor);
		
	alreadyHitActors.Add(hitCharacter);
	ActorsToProcess.Enqueue(hitCharacter);
	_initialHit = OtherActor;

	int numberOfForks = 0;

	DespawnProjectile();
	
	while (!ActorsToProcess.IsEmpty())
	{
		if (numberOfForks >= _projectileForking)
		{
			break;
		}
		lightningHits.Empty();
		AActor* Actor;
		ActorsToProcess.Dequeue(Actor);
		
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
		traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
		

		
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
		puddleObjectType, PuddleClass, alreadyHitActors, puddlesHits);
		

		for (AActor* hitActor : lightningHits)
		{
			alreadyHitActors.Add(hitActor);
			ActorsToProcess.Enqueue(hitActor);
			numberOfForks++;
		}

		for (AActor* hitPuddle : puddlesHits)
		{
			alreadyHitActors.Add(hitPuddle);
			ActorsToProcess.Enqueue(hitPuddle);
		}

		if (_overspillHits && ActorsToProcess.IsEmpty())
		{
			AuraCharacterClass = AAuraCharacter::StaticClass();

			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
			traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
			
			for (AActor* hitActor : lightningHits)
			{
				alreadyHitActors.Add(hitActor);
				ActorsToProcess.Enqueue(hitActor);
				numberOfForks++;
				if (numberOfForks >= _projectileForking)
				{
					break;
				}
			}
		}
		TArray<AActor*> forks;
		forks += lightningHits;
		forks += puddlesHits;
		
		_electricityForks.Add(Actor, FLightningStruct(forks));
		forks.Empty();
	}

	LightningHitEvent(_electricityForks);
	DealDamage(alreadyHitActors);
	
	
}
