#include "FireProjectile.h"

#include "AuraDataInclude.h"
#include "AuraCharacter.h"
#include "AuraHandler.h"
#include "ExplosiveBarrel.h"
#include "Kismet/GameplayStatics.h"

void AFireProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* AuraHandlerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass());

	if (AuraHandlerActor)
	{
		_handler = Cast<AAuraHandler>(AuraHandlerActor);
	}
}


void AFireProjectile::DealDamage(TArray<AActor*> hitCharacter)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Fire");
	int numberOfForks = 0;

	FireExplosion(hitCharacter[0]->GetActorLocation());
	
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
			barrel->Explode();
			barrel->Despawn();
			return;
		}
		else
		{
			barrel->SetFire();
			return;
		}
		
	}
	
	Character->QueueDamage(_explosionDamage, ElementTypes::FIRE);
	_handler->CastAuraByName("FIRE", Character, nullptr);
}
