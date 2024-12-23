#include "LingeringFire.h"

#include "AuraCharacter.h"
#include "AuraHandler.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ALingeringFire::ALingeringFire()
{
	_fireCollider = CreateDefaultSubobject<USphereComponent>("fire collider");
	RootComponent = _fireCollider;
	PrimaryActorTick.bCanEverTick = true;
}

void ALingeringFire::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorTickInterval(1);
	
	AActor* AuraHandlerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass());

	if (AuraHandlerActor)
	{
		_handler = Cast<AAuraHandler>(AuraHandlerActor);
	}
}

void ALingeringFire::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TArray<AActor*> overLappingActors;
	_fireCollider->GetOverlappingActors(overLappingActors, AAuraCharacter::StaticClass());
	
	for (AActor* Actor : overLappingActors)
	{
		if (Actor->IsA<AAuraCharacter>() && Actor!=this)
		{
			AAuraCharacter* Character = Cast<AAuraCharacter>(Actor);
			
			_handler->CastAuraByName("FIRE", Character, nullptr);
			Character->QueueDamage(_fireDamage, FIRE);
		}
	}
}

void ALingeringFire::DespawnFire()
{
	if (!this){return;}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ALingeringFire::SpawnFire(int fireRadius)
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	_fireCollider->SetSphereRadius(fireRadius);

	GetWorldTimerManager().SetTimer(DespawnHandle, FTimerDelegate::CreateLambda([this] {ALingeringFire::DespawnFire();}), _fireDuration, false);
	
}

void ALingeringFire::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DespawnHandle);
	Super::EndPlay(EndPlayReason);
}
