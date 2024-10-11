#include "ExplosiveBarrel.h"

#include "Aura.h"
#include "AuraHandler.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	_oilCollider = CreateDefaultSubobject<UBoxComponent>("oil Spill");
	_oilCollider->SetupAttachment(RootComponent);
}


void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	_oilCollider->Deactivate();
	
	GetMovementComponent()->Deactivate();

	AActor* AuraHandlerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass());

	if (AuraHandlerActor)
	{
		_auraHandler = Cast<AAuraHandler>(AuraHandlerActor);
	}

	if (!_auraHandler)
	{
		Despawn();
	}
}

void AExplosiveBarrel::Tick(float DeltaSeconds)
{
	UpdateAuras(DeltaSeconds);
	
	if (!GetStats()->_isAlive && !_oilSpilled)
	{
		SpillOil();
	}

	if (_oilSpilled && _onFire)
	{
		SpreadFire();
	}
}

void AExplosiveBarrel::Despawn()
{
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AExplosiveBarrel::SpillOil()
{
	_oilCollider->Activate();
	_oilSpilled = true;
	SpillOilEvent();
}

void AExplosiveBarrel::SpreadFire()
{
	TArray<AActor*> Actors;
	_oilCollider->GetOverlappingActors(Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->IsA<AAuraCharacter>() && Actor!=this)
		{
			AAuraCharacter* Character = Cast<AAuraCharacter>(Actor);

			
			_auraHandler->CastAuraByName("FIRE", Character, nullptr);
				
			
			
			
		}
	}
}

void AExplosiveBarrel::SetFire()
{
	_onFire = true;
	SetFireEvent();

	FTimerHandle Handle;

	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this](){EndFire();}), _burningDuration, false);
}

void AExplosiveBarrel::EndFire()
{
	Despawn();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "despawning fire");
}
