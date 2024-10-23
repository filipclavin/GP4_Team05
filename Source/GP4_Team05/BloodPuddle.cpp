#include "BloodPuddle.h"
#include "ChaosManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "ChaosManager.h"
#include "Engine/World.h"
#include "AuraHandler.h"
#include "Materials/MaterialInstanceDynamic.h"


ABloodPuddle::ABloodPuddle()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	
	_collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(_collisionBox);

	
	_puddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuddleMesh"));
	_puddleMesh->SetupAttachment(_collisionBox);
	_collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABloodPuddle::OnPlayerEnterPuddle);
	_collisionBox->OnComponentEndOverlap.AddDynamic(this, &ABloodPuddle::OnPlayerExitPuddle);


	_healingPercent = 1;
	_fadeSpeedStart = 0.2f;
	_fadeSpeedEnd = 1.5f;
	_shrinkSpeed = 0.1f;
	_opacity = 1.0f;
	_bPlayerOnPuddle = false;	
	_auraCharacter = nullptr;
	
}

void ABloodPuddle::BeginPlay()
{
	Super::BeginPlay();

	//_puddleTickDuration = _puddleTickInterval;

	_auraCharacter = Cast<AAuraCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	_auraHandler   = Cast<AAuraHandler>(UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass()));
	_chaosManager  = _auraCharacter->GetComponentByClass<UChaosManager>();
	
	if (_puddleMesh)
	{					 
		UMaterialInterface* Material = _puddleMesh->GetMaterial(0);
		if (Material)
		{
			_dynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			_puddleMesh->SetMaterial(0, _dynamicMaterial);
			_dynamicMaterial->SetScalarParameterValue(FName("Opacity"), 1.0f);
		}
	}
	SetActorScale3D(FVector(2.0f));
		
}
void ABloodPuddle::OnPlayerEnterPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		//if (!_bPlayerOnPuddle)
		//_auraHandler->CastAuraByName("Blood Offer", _auraCharacter, nullptr);
		_bPlayerOnPuddle = true;
	}
}
void ABloodPuddle::OnPlayerExitPuddle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		_bPlayerOnPuddle = false;
	}
}

void ABloodPuddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_bPlayerOnPuddle)
	{
		
		if(_puddleTickDuration <= 0)
		{
			ApplyHealing(DeltaTime);
			_puddleTickDuration = _puddleTickInterval;
			//_bPlayerOnPuddle = false;
		}
		_puddleTickDuration -= DeltaTime;
		
		FadeOut(DeltaTime);
		ShrinkPuddle(DeltaTime);
		//ApplyHealing(DeltaTime);

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
	if (!_auraCharacter && !_auraHandler)
		return;  
	
	// For healing percent based on Max HP you wanna go MaxHealth / _HealingPercent = healingValue! 
	//_auraCharacter->QueueHeal(_healingPercent);
	//if (_auraCharacter->GetStats()->_currentHealth == _auraCharacter->GetStats()->_maxHealth)
			{
			//	_chaosManager->addChaos();
			}
	_auraHandler->CastAuraByName("Blood Offer", _auraCharacter, nullptr);
	OnPlayerAbsorbingBlood();

}


