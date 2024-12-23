#include "Room.h"
#include "LevelGenerator.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AuraCharacter.h"
#include "BaseEnemyClass.h"
#include "BlueprintGameplayTagLibrary.h"
#include "EnemySpawner.h"
#include "NavigationSystem.h"
#include "ChaosManager.h"
#include "RoomAnchor.h"
#include "AI/NavigationSystemBase.h"

ARoom::ARoom()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>("Default Scene Component");
	_playerEnterTrigger = CreateDefaultSubobject<UBoxComponent>("Player Enter Trigger");
	_playerEnterTrigger->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	_playerEnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoom::BeginOverlap);

	_anchors = TArray<ARoomAnchor*>();
}

void ARoom::BeginPlay()
{
	Super::BeginPlay();

	_levelGenerator = Cast<ALevelGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()));
	if (_levelGenerator) 
	{
		if (_bridgeRoom)
			_colliderActiveOnSpawn = true; 

		_levelGenerator->SetCurrentRoom(this);
		OnRoomLoad();
		ActivateRoom();
	}
}

void ARoom::OnRoomComplete()
{
	if (!_roomIsCompleted) {
		//_levelGenerator->GetBridgeRoom()->_occupiedAnchor = -1;
		_levelGenerator->GetBridgeRoom()->GetUnusedAnchor();
		_levelGenerator->GetBridgeRoom()->AnchorToRoom(GetOccupiedAnchor(), this);
		_levelGenerator->GetBridgeRoom()->_hasEntered = false;
		_roomIsCompleted = true;

		_levelGenerator->GetBridgeRoom()->GetOccupiedAnchor()->OpenAnchorDoor();
		GetOccupiedAnchor()->OpenAnchorDoor();

		
	}
}

void ARoom::ResetChaosManager()
{
	if (_enemySpawner) 
	{
		_chaosManager->ResetChaosBarProgress();
		_chaosManager->ScaleChaosBar();
		_chaosManager->DisableChaosBar(true);
	}
	else 
		_chaosManager->DisableChaosBar(false);
}

void ARoom::OnRoomChaosBarFilled_Implementation()
{
	OnRoomComplete();
}

void ARoom::OnInteractablePickup_Implementation()
{
	OnRoomComplete();
}

void ARoom::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
 	if (!_hasEntered) {
		if (_colliderActiveOnSpawn) 
		{
			AAuraCharacter* check = Cast<AAuraCharacter>(OtherActor);
			if (!check)
				return;

			if (!check->IsPlayer())
				return;
		
			if (!_levelGenerator) {
				FetchLevelGenerator();
			}

			_hasEntered = true;

			if(GetOccupiedAnchor())
				GetOccupiedAnchor()->CloseAnchorDoor();

			if (_bridgeRoom)
			{
				_levelGenerator->LoadNewRoom();
			}
			else 
			{
				if(_levelGenerator->GetBridgeRoom()->GetOccupiedAnchor())
					_levelGenerator->GetBridgeRoom()->GetOccupiedAnchor()->CloseAnchorDoor();
				
				_chaosManager = check->GetComponentByClass<UChaosManager>();
				if(_chaosManager)
				   ResetChaosManager();

				GetUnusedAnchor();
			}

			OnPlayerEnter();
		}
	} 
}

void ARoom::ActivateRoom()
{
	_colliderActiveOnSpawn = true;

	if(_navMesh)
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->Build();
}

void ARoom::AnchorToRoom(const ARoomAnchor* anchor, const ARoom* room)
{
	if (anchor && !_anchors.IsEmpty()) {	
		ARoomAnchor* selectedAnchor = GetOccupiedAnchor();
		
		if (selectedAnchor) {
			SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
			SetActorLocation(FVector(0.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);

			FVector anchorNoZ = selectedAnchor->GetActorLocation();
			FVector roomNoZ   = GetActorLocation();
			anchorNoZ.Z		  = 0.0f;
			roomNoZ.Z		  = 0.0f;


			FVector dir = anchor->GetActorForwardVector();
			float distance = FVector::Distance(anchorNoZ, roomNoZ);
			dir *= distance;

			FVector zDelta = GetActorLocation();
			zDelta		  -= selectedAnchor->GetActorLocation();
			zDelta.X	   = 0.0f;
			zDelta.Y	   = 0.0f;

			SetActorLocation(anchor->GetActorLocation(),  false, nullptr, ETeleportType::TeleportPhysics);
			SetActorLocation(GetActorLocation() + dir,    false, nullptr, ETeleportType::TeleportPhysics);
			SetActorLocation(GetActorLocation() + zDelta, false, nullptr, ETeleportType::TeleportPhysics);

			FRotator newRotation;

			newRotation = anchor->GetActorRotation() - selectedAnchor->GetActorRotation();
			SetActorRotation(newRotation - FRotator(0.0f, 180.0f, 0.0f));
		}
		else
		{
			if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Failed to fetch Anchor, Spawning room at center of world!"));
			SetActorLocation({ 0.0f, 0.0f, 0.0f });
		}
	}
	else 
	{
		SetActorLocation({ 0.0f, 0.0f, 0.0f });
	}
}

ARoomAnchor* ARoom::GetUnusedAnchor()
{
	if (!_anchors.IsEmpty()) 
	{
		if (_occupiedAnchor == -1 && _prioritizeEntrance > -1) {
			if(_prioritizeEntrance >= _anchors.Num())
			{
				if(GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Invalid Priority Anchor Index!!"));
				
				_occupiedAnchor = 0;
				return _anchors[0];
			}
			_occupiedAnchor = _prioritizeEntrance;
			return _anchors[_prioritizeEntrance];
		}
		else {
			INT32 newAnchor = FMath::RandRange(0, _anchors.Num() - 1);
			if (newAnchor == _occupiedAnchor)
				newAnchor = newAnchor >= _anchors.Num() - 1 ? 0 : newAnchor + 1;

			_occupiedAnchor = newAnchor;

			return _anchors[newAnchor];
		}
	}
	else 
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No Anchors to attach to!"));
	}

	return nullptr;
}

ARoomAnchor* ARoom::GetOccupiedAnchor()
{
	if (_occupiedAnchor >= 0 && _occupiedAnchor < _anchors.Num())
		return _anchors[_occupiedAnchor];
	else
		return nullptr;
}

void ARoom::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if(!_roomIsCompleted)
	{
		if (_chaosManager)
			if (_chaosManager->ChaosBarIsFilled()) 
				OnRoomChaosBarFilled();
	}
}

void ARoom::FetchLevelGenerator()
{
	_levelGenerator = Cast<ALevelGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()));
}

void ARoom::ClearRoom()
{
	TArray<FTimerHandle> killTimers;
	int currentEnemy = 0;
	
	FGameplayTagQueryExpression Expression;
	Expression.AnyTagsMatch();
	Expression.AddTag(TAG_Alive);
                     		
	TArray<AActor*> livingEnemies;
	UBlueprintGameplayTagLibrary::GetAllActorsOfClassMatchingTagQuery(GetWorld(), ABaseEnemyClass::
	StaticClass(),FGameplayTagQuery::BuildQuery(Expression), livingEnemies);
	for (AActor* LivingEnemy : livingEnemies)
	{
		ABaseEnemyClass* Enemy = Cast<ABaseEnemyClass>(LivingEnemy);
		killTimers.Add(FTimerHandle());

		GetWorld()->GetTimerManager().SetTimer(killTimers[currentEnemy], FTimerDelegate::CreateLambda
			([Enemy](){Enemy->Kill();}), FMath::RandRange(0.f, 2.f), false);
		currentEnemy++;
	}
}