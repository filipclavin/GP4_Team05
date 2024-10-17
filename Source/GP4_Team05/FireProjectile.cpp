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
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "bigger explosion");
	}
	if (upgradeAmount >= lingeringFireThreshold)
	{
		_lingeringFire = true;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "lingering fire");
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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Fire");
	int numberOfForks = 0;

	FireExplosion(hitCharacter[0]->GetActorLocation());
	_lingeringFireActor->SetActorLocation(hitCharacter[0]->GetActorLocation());
	_lingeringFireActor->SpawnFire(_projectileExplosionRadius);
	
	
	for (AActor* hitActor : hitCharacter)
	{
    		
		if (hitActor != this)
		{
			
			dealFireDamage(Cast<AAuraCharacter>(hitActor));
			
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, hitActor->GetName() + " hit");
			numberOfForks++;

			if (numberOfForks >= _projectileForking)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "limit Reached");
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
	DrawDebugSphere(GetWorld(), explodeOrigin, _projectileExplosionRadius, 12, FColor::Red, true, 4.0f);

	for (AActor* HitEnemyActor : hitEnemies)
	{
		dealFireDamage(Cast<AAuraCharacter>(HitEnemyActor));
		
		
	}
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

	_owningPlayer->UpdateAurasOnAttackHits(Character, FIRE_ATTACK);
	
	Character->QueueDamage(_explosionDamage, ElementTypes::FIRE);
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
