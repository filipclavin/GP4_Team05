#include "FireProjectile.h"

#include "AuraDataInclude.h"
#include "AuraCharacter.h"
#include "AuraHandler.h"
#include "BaseEnemyClass.h"
#include "ExplosiveBarrel.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void AFireProjectile::SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer)
{
	Super::SpawnProjectile(upgradeAmount, owningPlayer);
	
	if (upgradeAmount >= biggerOilExplosionThreshold)
	{
		_biggerOilExplosion = true;
	}
	if (upgradeAmount >= lingeringFireThreshold)
	{
		_lingeringFire = true;
	}
}


void AFireProjectile::BeginPlay()
{
	Super::BeginPlay();
	

	_lingeringFireActor = GetWorld()->SpawnActor<ALingeringFire>(lingeringFire);
	_lingeringFireActor->DespawnFire();
}

void AFireProjectile::DealDamage(TArray<AActor*> hitCharacter)
{
	int numberOfForks = 0;

	FireExplosion(hitCharacter[0]->GetActorLocation());
	if (_lingeringFire)
	{
		_lingeringFireActor->SetActorLocation(hitCharacter[0]->GetActorLocation());
		_lingeringFireActor->SpawnFire(_projectileExplosionRadius/2);
	}
	
	
	for (AActor* hitActor : hitCharacter)
	{
    		
		if (hitActor != this)
		{
			
			dealFireDamage(Cast<AAuraCharacter>(hitActor));
			
			numberOfForks++;

			if (numberOfForks >= _projectileForking)
			{
				break;
			}
		}
    			
	}
}

void AFireProjectile::FireExplosion(FVector explodeOrigin)
{
	TArray<AActor*> hitEnemies;
	TArray<AActor*> IgnoreArray;

	//ignore player
	IgnoreArray.Add(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), explodeOrigin, _projectileExplosionRadius,
		traceObjectTypes, AAuraCharacter::StaticClass(), IgnoreArray, hitEnemies);

	for (AActor* HitEnemyActor : hitEnemies)
	{
		dealFireDamage(Cast<AAuraCharacter>(HitEnemyActor));
		
		
	}
	fireHitEvent(explodeOrigin, _projectileExplosionRadius, hitEnemies, _electricityForks);
}

void AFireProjectile::dealFireDamage(AAuraCharacter* Character)
{
	if (Character->IsA<AExplosiveBarrel>())
	{
		AExplosiveBarrel* barrel = Cast<AExplosiveBarrel>(Character);
		if (!barrel->_oilSpilled)
		{
			barrel->Explode(_biggerOilExplosion);
			barrel->Despawn();
			return;
		}
		else
		{
			barrel->SetFire();
			return;
		}
		
	}

	int damage = Character->QueueDamage(_explosionDamage, ElementTypes::FIRE, _owningPlayer);
	_owningPlayer->UpdateAurasOnAttackHits(Character, FIRE_ATTACK, damage);
	
	_handler->CastAuraByName("FIRE", Character, nullptr);
}

void AFireProjectile::SetOnFire(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ABaseEnemyClass::StaticClass()))
	{
		dealFireDamage(Cast<AAuraCharacter>(OtherActor));
	}
}
