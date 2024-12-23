// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"

#include <string>

#include "ChaosManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "FrameTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectileBaseClass.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	_cameraArmComponent = CreateDefaultSubobject<USpringArmComponent>("Camera arm");
	_cameraArmComponent->SetupAttachment(GetCapsuleComponent());
	_cameraArmComponent->bUsePawnControlRotation = true;
	
	_playerCameraComponent = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	_playerCameraComponent->SetupAttachment(_cameraArmComponent);

	_meleeHitbox = CreateDefaultSubobject<UBoxComponent>("melee hitbox");
	_meleeHitbox->SetupAttachment(GetCapsuleComponent());

	_dashHitbox = CreateDefaultSubobject<USphereComponent>("dash hitbox");
	_dashHitbox->SetupAttachment(RootComponent);

	
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	
	if (_move)
	{
		Input->BindAction(_move, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveAction);
		Input->BindAction(_move, ETriggerEvent::None	 , this, &APlayerCharacter::ResetInputVector);
	}
	if (_look)
	{
		Input->BindAction(_look, ETriggerEvent::Triggered, this, &APlayerCharacter::LookAction);
	}
	if (_jump)
	{
		Input->BindAction(_jump, ETriggerEvent::Started, this, &APlayerCharacter::JumpAction);
	}
	if (_attack)
	{
		Input->BindAction(_attack, ETriggerEvent::Completed, this, &APlayerCharacter::MeleeAction);
		Input->BindAction(_attack, ETriggerEvent::Started,   this, &APlayerCharacter::BeginMeleeAction);
		//Input->BindAction(_attack, ETriggerEvent::Started,   this, &APlayerCharacter::RangeAttackAction);
	}
	if (_aim)
	{
		Input->BindAction(_aim, ETriggerEvent::Started,	  this, &APlayerCharacter::BeginAimAction);
		Input->BindAction(_aim, ETriggerEvent::Completed, this, &APlayerCharacter::StopAimAction);
	}
	if (_dash)
	{
		Input->BindAction(_dash, ETriggerEvent::Started, this, &APlayerCharacter::DashAction);
	}
	if (_fireSelect && _lightningSelect && _rangeAttack && _bloodSelect)
	{
		Input->BindAction(_fireSelect,		ETriggerEvent::Started, this, &APlayerCharacter::FireAction);
		Input->BindAction(_lightningSelect, ETriggerEvent::Started, this, &APlayerCharacter::lightningAction);
		Input->BindAction(_bloodSelect	  , ETriggerEvent::Started, this, &APlayerCharacter::BloodAction);
		Input->BindAction(_rangeAttack,		ETriggerEvent::Started, this, &APlayerCharacter::RangeAttackAction);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	_dashHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::HandleDashHits);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_defaultInputMapping.LoadSynchronous(), 0);
		}
	}

	//create a semi projectile pool. considering the relatively close quarters a few seconds should be enough to 
	for (int i = 0; i < 10; i++)
	{
		_pooledElectricProjectiles.Add(GetWorld()->SpawnActor<AProjectileBaseClass>(_electricProjectile));
		_pooledElectricProjectiles[i]->SpawnProjectile(_electricLevel, this);
		_pooledElectricProjectiles[i]->DespawnProjectile();
		_pooledFireProjectiles.Add(GetWorld()->SpawnActor<AProjectileBaseClass>(_fireProjectile));
		_pooledFireProjectiles[i]->SpawnProjectile(_fireLevel, this);
		_pooledFireProjectiles[i]->DespawnProjectile();
	}

	_bloodProjectileToUse = GetWorld()->SpawnActor<AProjectileBaseClass>(_bloodProjectile);
	_bloodProjectileToUse->SpawnProjectile(_bloodLevel, this);
	_bloodProjectileToUse->DespawnProjectile();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	_meleeCooldownTimer += DeltaSeconds;
	_rangeCooldownTimer	+= DeltaSeconds;

	if (_chargingAttack)
	{
		_meleeHeavyTimer += DeltaSeconds;
	}
	
	if (CurrentlyDashing)
	{
		
		//Lerp the velocity to give some good easing -Gustav
		GetMovementComponent()->Velocity = FMath::Lerp(FVector::Zero(), _dashDirection, easeInOutQuint
			(GetWorld()->GetTimerManager().GetTimerElapsed(_dashHandle)/_dashDuration));
	}

	UpdateAuras(DeltaSeconds);
}

float APlayerCharacter::easeInOutQuint(float x)
{
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - FMath::Pow(-2 * x + 2, 5) / 2;
}


void APlayerCharacter::MoveAction(const FInputActionValue& Value)
{
	FVector2d moveVector = Value.Get<FVector2d>();
	_inputVector = FVector(moveVector, 0);
	
	MoveForwards(moveVector.Y);
	MoveSideways(moveVector.X);
}

void APlayerCharacter::ResetInputVector(const FInputActionValue& Value)
{
	_inputVector = FVector::Zero();
}

void APlayerCharacter::MoveForwards(float val)
{
	AddMovementInput(GetActorForwardVector(), val);
}

void APlayerCharacter::MoveSideways(float val)
{
	AddMovementInput(GetActorRightVector(), val);
}

void APlayerCharacter::LookAction(const FInputActionValue& Value)
{
	FVector2d lookVector = Value.Get<FVector2d>();

	LookUp(lookVector.Y);
	LookRight(lookVector.X);
}

void APlayerCharacter::LookRight(float val)
{
	AddControllerYawInput(-val);
}

void APlayerCharacter::LookUp(float val)
{
	AddControllerPitchInput(val);
}

void APlayerCharacter::JumpAction(const FInputActionValue& Value)
{
	Jump();
}

void APlayerCharacter::BeginMeleeAction(const FInputActionValue& Value)
{
	if (_aiming) {return;}
	if (_meleeCooldown > _meleeCooldownTimer){GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown"); return;}
	if (_attackPending) {return;}

	_attackPending = true;
	_chargingAttack = true;
	
	BeginMeleeAttackEvent();
}

void APlayerCharacter::MeleeAction(const FInputActionValue& Value)
{
	if (!_chargingAttack){return;}
	if (_aiming) {return;}
	if (_meleeCooldown > _meleeCooldownTimer){GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown"); return;}
	

	_attackPending = true;
	
	UpdateAurasOnAttackCast(MELEE);

	float damage = _heavyAttackMeleeTime < _meleeHeavyTimer ? _heavyAttackMeleeDamage : _lightAttackMeleeDamage;

	_chargingAttack = false;

	if (_heavyAttackMeleeTime < _meleeHeavyTimer)
	{
		HeavyMeleeAttackEvent();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "heavy melee attack, dealing: " + FString::SanitizeFloat(damage) + "damage");
	}
	else
	{
		MeleeAttackEvent();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "light melee attack, dealing: " + FString::SanitizeFloat(damage) + "damage");
	}
}

void APlayerCharacter::DealMeleeDamage()
{
	float damage = _heavyAttackMeleeTime < _meleeHeavyTimer ? (_heavyAttackMeleeDamage/2) : _lightAttackMeleeDamage;

	_attackPending = false;
	
	_meleeCooldownTimer = 0;
	_meleeHeavyTimer = 0;	
	
	TArray<AActor*> HitActors;
	_meleeHitbox->GetOverlappingActors(HitActors);

	
	for (AActor* HitActor : HitActors)
	{
		if (HitActor != this)
		{
			if (HitActor->IsA<AAuraCharacter>())
			{
				AAuraCharacter* target = Cast<AAuraCharacter>(HitActor);
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, HitActor->GetName() + " hit");
				int newDamage = target->QueueDamage(damage, PHYSICAL, this);
				UpdateAurasOnAttackHits(target, MELEE, newDamage);
			}
		}
	}
}


void APlayerCharacter::lightningAction(const FInputActionValue& Value)
{
	_useElectric = true;
	chosenAttack = 0;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "using electricity");
}

void APlayerCharacter::FireAction(const FInputActionValue& Value)
{
	_useElectric = false;
	chosenAttack = 1;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "using fire");
}

void APlayerCharacter::BloodAction(const FInputActionValue& Value)
{
	
	chosenAttack = 2;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "using blood");
}


void APlayerCharacter::RangeAttackAction(const FInputActionValue& Value)
{
	
	if (_rangeCooldown > _rangeCooldownTimer) {GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown");return;}
	
	_rangeCooldownTimer = 0;
	
	RangedAttackEvent();

	int projectileRange;
	
	if (_useElectric){projectileRange = _pooledElectricProjectiles[_electricProjectileToUse]->_projectileRange;}
	else{projectileRange = _pooledFireProjectiles[_fireProjectileToUse]->_projectileRange;}

	FVector AimStart;
	FRotator CamRot;
	GetController<APlayerController>()->GetPlayerViewPoint(AimStart, CamRot);

	FVector CamForward = CamRot.Quaternion().GetForwardVector();
		
	FHitResult AimHit;
	GetWorld()->LineTraceSingleByChannel(AimHit,AimStart, AimStart +
		CamForward*projectileRange,ECC_Visibility);
	
	
	FVector AiMPoint = AimHit.bBlockingHit ? AimHit.Location: AimHit.TraceEnd;
	
	FVector BulletOrg = GetActorLocation();

	FVector BulletDir = _playerCameraComponent->GetForwardVector();

	if (FVector::Dist(AiMPoint, BulletOrg) > 300)
	{
		BulletDir = AiMPoint - BulletOrg;
	}

	


	if (chosenAttack == 0)
	{
		_pooledElectricProjectiles[_electricProjectileToUse]->SetActorLocationAndRotation
		(GetActorLocation() + GetActorForwardVector()*200.f ,UKismetMathLibrary::MakeRotFromX(BulletDir));
		_pooledElectricProjectiles[_electricProjectileToUse]->SpawnProjectile(_electricLevel, this);
        QueueDamage(_ligtningSelfDamage, PHYSICAL);
        
		_electricProjectileToUse++;
        
		if (_electricProjectileToUse > _pooledElectricProjectiles.Num()-1)
		{
			_electricProjectileToUse = 0;
		}
		
		UpdateAurasOnAttackCast(LIGHTNING_ATTACK);
		ElectricAttackEvent();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::FromInt(_electricProjectileToUse));
	
}
	else if (chosenAttack == 1)
	{
        QueueDamage(_fireSelfDamage, PHYSICAL);
		
		_pooledFireProjectiles[_fireProjectileToUse]->SetActorLocationAndRotation
		(GetActorLocation() + GetActorForwardVector()*200.f ,UKismetMathLibrary::MakeRotFromX(BulletDir));
		_pooledFireProjectiles[_fireProjectileToUse]->SpawnProjectile(_fireLevel, this);
        
        
		_fireProjectileToUse++;
		
        
		if (_fireProjectileToUse > _pooledFireProjectiles.Num()-1)
		{
			_fireProjectileToUse = 0;
		}
		
		UpdateAurasOnAttackCast(FIRE_ATTACK);
		FireAttackEvent();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::FromInt(_fireProjectileToUse));
	}
	else if (chosenAttack == 2)
	{
		QueueDamage(_bloodSelfDamage*(1/(1+_bloodLevel)), PHYSICAL);
		_bloodProjectileToUse->SpawnProjectile(_bloodLevel);
		BloodAttackEvent();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Blood attack");
	}
	
}



void APlayerCharacter::BeginAimAction(const FInputActionValue& Value)
{
	_aiming = true;

	BeginAimEvent();
	
}

void APlayerCharacter::StopAimAction(const FInputActionValue& Value)
{
	_aiming = false;

	StopAimEvent();
}

void APlayerCharacter::DashAction(const FInputActionValue& Value)
{
	
	if (CurrentlyDashing){GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "already dashing");return;}

	FVector dashDirection = _inputVector.Y*GetActorForwardVector()+_inputVector.X*GetActorRightVector(); 

	if (dashDirection == FVector::Zero())
	{
		dashDirection = GetActorForwardVector();
	}

	
	QueueDamage(_dashDamage, PHYSICAL);
	
	GetCapsuleComponent()->SetCollisionProfileName("HitWorldStaticOnly");

	dashDirection.Normalize();
	
	CurrentlyDashing = true;
	_dashDirection  = dashDirection*_dashSpeed;
	_damageTakenStorage = GetStats()->_allDamageTaken;
	GetStats()->_allDamageTaken = 0;
	
	UpdateAurasOnAttackCast(DASH);

	GetWorld()->GetTimerManager().SetTimer(_dashHandle, FTimerDelegate::CreateLambda([this] {APlayerCharacter::ResetDash();}), _dashDuration, false);
}

void APlayerCharacter::ResetDash()
{
	CurrentlyDashing = false;
	//GetCharacterMovement()->Velocity = FVector::Zero();
	GetCharacterMovement()->Velocity = _inputVector*GetActorForwardVector()*GetCharacterMovement()->MaxWalkSpeed;
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	_dashHitActors.Empty();
	GetStats()->_allDamageTaken = _damageTakenStorage;
}

void APlayerCharacter::HandleDashHits(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool ,bool bFromSweep,const FHitResult& SweepResult)
{
	
	if (!_dashHitActors.Contains(OtherActor) && OtherActor != this && CurrentlyDashing)
	{
		FVector rightOfDash = _dashDirection.Cross(FVector::UpVector);
		
		FVector knockbackDirection = rand() % 2 ? rightOfDash:-rightOfDash;

		knockbackDirection.Normalize();
		
		AAuraCharacter* HitCharacter = Cast<AAuraCharacter>(OtherActor);
		
		OtherActor->SetActorLocation(OtherActor->GetActorLocation() + knockbackDirection*_dashKnockBack);
		int damage = HitCharacter->QueueDamage(_dashDamage, ElementTypes::PHYSICAL, this);

		UpdateAurasOnAttackHits(HitCharacter, DASH, damage);
		_dashHitActors.Add(OtherActor);
	}
}


void APlayerCharacter::UpgradePlayer()
{
	if (Upgrades.Num() > _playerLevel)
	{
		GetStats()->_healingTaken += Upgrades[_playerLevel].BloodAbsorb;
     	GetComponentByClass<UChaosManager>()->ScaleChaosGain(Upgrades[_playerLevel].BloodAbsorb);
     	_dashSpeed += Upgrades[_playerLevel].DashSpeed;
     	_lightAttackMeleeDamage += Upgrades[_playerLevel].LightMeleeDamage;
     	_heavyAttackMeleeDamage += Upgrades[_playerLevel].HeavyMeleeDamage;
     	_playerLevel++;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "out of upgrades");
	}
	
}

void APlayerCharacter::Die()
{
	PlayerDeathEvent();
	
	Super::Die();
	
}
