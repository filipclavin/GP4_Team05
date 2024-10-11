#include "Room.h"
#include "LevelGenerator.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AuraCharacter.h"

ARoom::ARoom()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>("Default Scene Component");
	_playerEnterTrigger = CreateDefaultSubobject<UBoxComponent>("Player Enter Trigger");
	_playerEnterTrigger->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	_playerEnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoom::BeginOverlap);

	
}

void ARoom::BeginPlay()
{
	Super::BeginPlay();

	_levelGenerator = Cast<ALevelGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()));
	if (_levelGenerator) 
	{
		_levelGenerator->SetCurrentRoom(this);
		_colliderActiveOnSpawn = true;

		OnRoomLoad();
	}
}

void ARoom::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!_hasEntered) {
		if (_colliderActiveOnSpawn) 
		{
			AAuraCharacter* check = Cast<AAuraCharacter>(OtherActor);
			if (!check || !check->IsPlayer())
				return;
		
			if (!_levelGenerator) {
				FetchLevelGenerator();
			}
			
			_hasEntered = true;

			if (_bridgeRoom)
				_levelGenerator->LoadNewRoom();
			else
			{
				_levelGenerator->GetBridgeRoom()->_occupiedAnchor = -1;
				_levelGenerator->GetBridgeRoom()->_hasEntered	  = false;
				_levelGenerator->GetBridgeRoom()->AnchorToRoom(GetUnusedAnchor(), this);	
			}

			OnPlayerEnter();
		}
	}
}

void ARoom::AnchorToRoom(const AActor* anchor, const ARoom* room)
{
	if (anchor && !_anchors.IsEmpty()) {	
		AActor* selectedAnchor = GetUnusedAnchor();
		
		SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		
		FVector dir = anchor->GetActorForwardVector();
		float distance = FVector::Distance(selectedAnchor->GetActorLocation(), GetActorLocation());
		dir *= distance;
		SetActorLocation(anchor->GetActorLocation());
		SetActorLocation(GetActorLocation() + dir);

		FRotator newRotation;

		newRotation = anchor->GetActorRotation() + selectedAnchor->GetActorRotation();
		SetActorRotation(newRotation - FRotator(0.0f, 180.0f, 0.0f));
	}
	else
		SetActorLocation({ 0.0f, 0.0f, 0.0f });
}

AActor* ARoom::GetUnusedAnchor()
{
	if (!_anchors.IsEmpty()) 
	{
		INT32 newAnchor = FMath::RandRange(0, _anchors.Num() - 1);
		if (newAnchor == _occupiedAnchor)
			newAnchor = newAnchor >= _anchors.Num() - 1 ? 0 : newAnchor + 1;
		
		if(_occupiedAnchor < 0)
			_occupiedAnchor = newAnchor;

		return _anchors[newAnchor];
	}
	else 
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No Anchors to attach to!"));
	}

	return nullptr;
}

AActor* ARoom::GetOccupiedAnchor()
{
	if (_occupiedAnchor > 0)
		return _anchors[_occupiedAnchor];
	else
		return nullptr;
}

void ARoom::FetchLevelGenerator()
{
	_levelGenerator = Cast<ALevelGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()));
}

