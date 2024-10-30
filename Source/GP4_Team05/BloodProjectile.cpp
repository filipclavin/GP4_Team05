#include "BloodProjectile.h"

#include "AuraHandler.h"
#include "BaseEnemyClass.h"
#include "ChaosManager.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"

ABloodProjectile::ABloodProjectile()
{
	coneRoot = CreateDefaultSubobject<USceneComponent>	   ("cone root");
	coneRoot->SetupAttachment(RootComponent);
	coneMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cone mesh");
	coneMesh->SetupAttachment(coneRoot);
}


void ABloodProjectile::SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer)
{
	Super::SpawnProjectile(upgradeAmount, owningPlayer);
	_durationTimer = 0;
	_enemiesHit	   = 0;
	hitActors.Empty();
	_projectileCollider->SetSphereRadius(_projectileRange);
	coneRoot->SetRelativeScale3D(FVector(_projectileRange/100.f));

	_owningPlayer->UpdateAurasOnAttackCast(BLOOD_ATTACK);

	if (upgradeAmount >= AddArmorThreshold)
	{
		addArmor = true;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "gain armor");
	}

	if (upgradeAmount >= GainHealthThreshold)
	{
		healPlayer = true;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "heal player");
	}
}


void ABloodProjectile::Tick(float DeltaSeconds)
{
	SetActorLocationAndRotation(_owningPlayer->GetActorLocation(), _owningPlayer->GetActorRotation());

	_durationTimer += DeltaSeconds;

	if (_durationTimer > abilityDuration)
	{
		bloodHitEvent(hitActors.Array());
		DespawnProjectile();
	}

	TArray<AActor*> overlappingActors;
	_projectileCollider->GetOverlappingActors(overlappingActors, ABaseEnemyClass::StaticClass());

	if (!overlappingActors.IsEmpty())
	{
		if (addArmor)
		{
		_handler->CastAuraByName("BLOOD ARMOR", _owningPlayer, nullptr);
			
		}
		DealDamage(overlappingActors);
		
	}
	
}


void ABloodProjectile::HitEnemies(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABloodProjectile::DealDamage(TArray<AActor*> hitCharacter)
{
	for (AActor* hitActor : hitCharacter)
	{
		if (_enemiesHit >= _hitEnemiesCap)
		{
			return;
			
		}
		if (!hitActors.Contains(hitActor))
		{
			FVector enemyDirection = (hitActor->GetActorLocation()-GetActorLocation());
			enemyDirection.Normalize();
			float AimDotProduct = FVector::DotProduct(GetActorForwardVector(), enemyDirection);
			if (AimDotProduct > 1.f-AttackConeAngle)
			{
				AAuraCharacter* hitCharacter = Cast<AAuraCharacter>(hitActor);
				if (hitCharacter->GetStats()->_isAlive)
				{
					hitCharacter->QueueDamage(_projectileDamage, BLOOD, _owningPlayer->GetStats());
                                    
					_owningPlayer->UpdateAurasOnAttackHits(hitCharacter, BLOOD_ATTACK, _projectileDamage);
                                    				
					GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, hitActor->GetName() + " Hit");
                                    
					_chaosManager->addChaos(ChaosAddPerHit);
					if (healPlayer)
					{
						_owningPlayer->QueueHeal(healPerHit);
					}
                                    				
					hitActors.Emplace(hitActor);
					_enemiesHit++;
				}
				
				
			}
			
		}
	}
}

