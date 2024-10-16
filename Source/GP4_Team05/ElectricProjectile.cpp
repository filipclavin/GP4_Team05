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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "filtering enemy type");
	}
	if (upgradeAmount >= enemyHitOverspillThreshold)
	{
		_overspillHits = true;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "overspilling hits");
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

	int numberOfForks = 0;

	DespawnProjectile();
	
	while (!ActorsToProcess.IsEmpty())
	{
		if (numberOfForks >= _projectileForking)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "limit Reached");
			break;
		}
		lightningHits.Empty();
		AActor* Actor;
		ActorsToProcess.Dequeue(Actor);
		
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
		traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
		DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange, 12, FColor::Red, true, 4.0f);
		

		
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
		puddleObjectType, PuddleClass, alreadyHitActors, puddlesHits);
		DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange, 12, FColor::Red, true, 4.0f);
		

		for (AActor* hitActor : lightningHits)
		{
			DrawDebugLine(GetWorld(), Actor->GetActorLocation(), hitActor->GetActorLocation(),
				FColor::Yellow, true, 4, 0, 1);
			alreadyHitActors.Add(hitActor);
			ActorsToProcess.Enqueue(hitActor);
			numberOfForks++;
		}

		for (AActor* hitPuddle : puddlesHits)
		{
			DrawDebugLine(GetWorld(), Actor->GetActorLocation(), hitPuddle->GetActorLocation(),
				FColor::Yellow, true, 4, 0, 1);
			alreadyHitActors.Add(hitPuddle);
			ActorsToProcess.Enqueue(hitPuddle);
		}

		if (_overspillHits && ActorsToProcess.IsEmpty())
		{
			AuraCharacterClass = AAuraCharacter::StaticClass();

			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
			traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange, 12, FColor::Red, true, 4.0f);
			
			for (AActor* hitActor : lightningHits)
			{
				DrawDebugLine(GetWorld(), Actor->GetActorLocation(), hitActor->GetActorLocation(),
					FColor::Yellow, true, 4, 0, 1);
				alreadyHitActors.Add(hitActor);
				ActorsToProcess.Enqueue(hitActor);
				numberOfForks++;
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,
		FString::FromInt(numberOfForks)+" of " +FString::FromInt(_projectileForking));

	DealDamage(alreadyHitActors);
	
	
}
