#include "BloodPuddle.h"
#include "ChaosManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "ChaosManager.h"
#include "Materials/MaterialInstanceDynamic.h"


ABloodPuddle::ABloodPuddle()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	
	_puddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuddleMesh"));
	SetRootComponent(_puddleMesh);

	_collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	_collisionBox->SetupAttachment(RootComponent);
	_collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABloodPuddle::OnPlayerEnterPuddle);

	
	//_fadeSpeed = 0.5f;
	_fadeSpeedStart = 0.2f;
	_fadeSpeedEnd = 1.5f;
	_shrinkSpeed = 0.1f;
	_opacity = 1.0f;
	_bPlayerOnPuddle = false;
	_healingProgress = 0.0f;
	// Heals 30% of max health
	_healingPercentage = 0.30f;
	// Heals at 10 points per second 
	_healingRate = 10.0f;
	_auraCharacter = nullptr;
}

void ABloodPuddle::BeginPlay()
{
	Super::BeginPlay();

	_auraCharacter = Cast<AAuraCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	_chaosManager = _auraCharacter->GetComponentByClass<UChaosManager>();
	
	
	if (_puddleMesh)
	{					 
		UMaterialInterface* Material = _puddleMesh->GetMaterial(0);
		if (Material)
		{
			_dynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			_puddleMesh->SetMaterial(0, _dynamicMaterial);
		}
	}
	_opacity = 1.0f;
	
}
void ABloodPuddle::OnPlayerEnterPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		_bPlayerOnPuddle = true;
	}
}


void ABloodPuddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_bPlayerOnPuddle)
	{
		ApplyHealing(DeltaTime);
		FadeOut(DeltaTime);
		ShrinkPuddle(DeltaTime);
	}

}

void ABloodPuddle::FadeOut(float DeltaTime)
{
	if (_dynamicMaterial && _opacity > 0.0f)
	{
		float _fadeSpeed = _opacity > 0.5f ? _fadeSpeedStart : _fadeSpeedEnd;
		_opacity = FMath::FInterpTo(_opacity, 0.0f, DeltaTime, _fadeSpeed);
		_dynamicMaterial->SetScalarParameterValue(FName("Opacity"), _opacity);
		if (_opacity <= 0.01f){Destroy();}
	}
}
void ABloodPuddle::ShrinkPuddle(float DeltaTime)
{
	if (_puddleMesh)
	{
		FVector CurrentScale = _puddleMesh->GetComponentScale();
		if (CurrentScale.X > 0.1f)  // Ensures it doesn't shrink too much
		{
			FVector NewScale = FMath::VInterpTo(CurrentScale, FVector(0.1f), DeltaTime, _shrinkSpeed);
			_puddleMesh->SetWorldScale3D(NewScale);
		}
	}
}

void ABloodPuddle::ApplyHealing(float DeltaTime)
{
	if (!_auraCharacter)
	{
		return;  
	}
	
	float _maxHealing = 100.0f * _healingPercentage;
	if (_healingProgress < _maxHealing)
	{   
		float HealThisTick = FMath::Min(_healingRate * DeltaTime, _maxHealing - _healingProgress);
		_healingProgress += HealThisTick;
		int HealAmount = FMath::RoundToInt(HealThisTick);

		//this is just for a temporary implementation of adding chaos, CHANGE LATER! -Gustav
		if (_auraCharacter->GetStats()->_currentHealth == _auraCharacter->GetStats()->_maxHealth)
		{
			_chaosManager->bloodPickup();
		}
		_auraCharacter->QueueHeal(HealThisTick);

		UE_LOG(LogTemp, Warning, TEXT("Healed: %f, Total Healed: %f"), HealThisTick, _healingProgress);
					
	}
}
ABloodPuddle* ABloodPuddle::SpawnPuddle(UWorld* World, FVector Location, FRotator Rotation)
{
	if (World)
	{
		return World->SpawnActor<ABloodPuddle>(ABloodPuddle::StaticClass(), Location, Rotation);
	}
	return nullptr;
}

