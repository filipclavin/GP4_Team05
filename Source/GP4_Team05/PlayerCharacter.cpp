// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"

#include <string>

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
		Input->BindAction(_attack, ETriggerEvent::Started,   this, &APlayerCharacter::RangeAttackAction);
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
	if (_fireCone)
	{
		Input->BindAction(_fireCone, ETriggerEvent::Ongoing, this, &APlayerCharacter::FireConeAction);
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

	_chargingAttack = true;
	
	BeginMeleeAttackEvent();
}

void APlayerCharacter::MeleeAction(const FInputActionValue& Value)
{
	if (!_chargingAttack){return;}
	if (_aiming) {return;}
	if (_meleeCooldown > _meleeCooldownTimer){GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown"); return;}
	

	

	float damage = _heavyAttackMeleeTime < _meleeHeavyTimer ? _heavyAttackMeleeDamage : _lightAttackMeleeDamage;

	if (_heavyAttackMeleeTime < _meleeHeavyTimer)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "heavy melee attack, dealing: " + FString::SanitizeFloat(damage) + "damage");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "light melee attack, dealing: " + FString::SanitizeFloat(damage) + "damage");
	}
	_meleeCooldownTimer = 0;
	_meleeHeavyTimer = 0;
	_chargingAttack = false;
	MeleeAttackEvent();
	
	TArray<AActor*> HitActors;
	_meleeHitbox->GetOverlappingActors(HitActors);

	
	for (AActor* HitActor : HitActors)
	{
		if (HitActor != this)
		{
			if (HitActor->IsA<AAuraCharacter>())
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, HitActor->GetName() + " hit");
				Cast<AAuraCharacter>(HitActor)->QueueDamage(_lightAttackMeleeDamage, PHYSICAL);
			}
		}
		
	}
}

void APlayerCharacter::RangeAttackAction(const FInputActionValue& Value)
{
	if (!_aiming) {return;}
	if (_rangeCooldown > _rangeCooldownTimer) {GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown");return;}
	
	_rangeCooldownTimer = 0;

	RangedAttackEvent();
	//for now i will convert the ranged attack to arcing surge

	TArray<AActor*>	lightningHits;
	TArray<AActor*>	alreadyHitActors;
	TQueue<AActor*> ActorsToProcess;

	UClass* AuraCharacterClass = AAuraCharacter::StaticClass();
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	FHitResult lightningHit;
	GetWorld()->LineTraceSingleByObjectType(lightningHit, _playerCameraComponent->GetComponentToWorld().GetLocation() + _playerCameraComponent->GetForwardVector()*
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2,_playerCameraComponent->GetComponentToWorld().GetLocation() + _playerCameraComponent->GetForwardVector()*_range,ECC_Pawn);

	if (!lightningHit.bBlockingHit){return;}
	
	if (lightningHit.GetActor()->IsA<AAuraCharacter>())
	{
		AAuraCharacter* hitCharacter = Cast<AAuraCharacter>(lightningHit.GetActor());
		
		alreadyHitActors.Add(hitCharacter);
		ActorsToProcess.Enqueue(hitCharacter);

		while (!ActorsToProcess.IsEmpty())
		{
			lightningHits.Empty();
			AActor* Actor;
			ActorsToProcess.Dequeue(Actor);
			
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _spreadRadius,
				traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), _spreadRadius, 12, FColor::Red, true, 4.0f);

			for (AActor* hitActor : lightningHits)
			{
				DrawDebugLine(GetWorld(), Actor->GetActorLocation(), hitActor->GetActorLocation(), FColor::Yellow, true, 4, 0, 1);
				alreadyHitActors.Add(hitActor);
				ActorsToProcess.Enqueue(hitActor);
			}
			
		}

		for (AActor* hitActor : alreadyHitActors)
		{
			if (hitActor != this)
			{
				//TODO change to lightning when its ready
				Cast<AAuraCharacter>(hitActor)->QueueDamage(_Ligtningdamage, ElementTypes::WATER);
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, hitActor->GetName() + " hit");
			}
			
		}
		
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

	GetCapsuleComponent()->SetCollisionProfileName("HitWorldStaticOnly");

	dashDirection.Normalize();
	
	CurrentlyDashing = true;
	_dashDirection  = dashDirection*_dashSpeed;
	
	GetWorld()->GetTimerManager().SetTimer(_dashHandle, FTimerDelegate::CreateLambda([this] {APlayerCharacter::ResetDash();}), _dashDuration, false);
}

void APlayerCharacter::ResetDash()
{
	CurrentlyDashing = false;
	GetCharacterMovement()->Velocity = FVector::Zero();
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	_dashHitActors.Empty();
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
		HitCharacter->QueueDamage(_dashDamage, ElementTypes::PHYSICAL);

		
		_dashHitActors.Add(OtherActor);
	}
}

void APlayerCharacter::FireConeAction(const FInputActionValue& Value)
{
	TArray<AActor*> HitActors;
	TArray<AActor*> IgnoredActors;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), _playerCameraComponent->GetComponentLocation(), _rangeOfFireCone,
				traceObjectTypes, AAuraCharacter::StaticClass(), IgnoredActors, HitActors);
	
	for (AActor* HitActor : HitActors)
	{
		if (HitActor != this)
		{
			FVector hitActorRelativeLoc = (HitActor->GetActorLocation()-_playerCameraComponent->GetComponentLocation());
			hitActorRelativeLoc.Normalize();
			float hitActorDotProduct = FVector::DotProduct(hitActorRelativeLoc,_playerCameraComponent->GetForwardVector());


			if (hitActorDotProduct > 1-_widthOfFireConeRadians)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,FString::SanitizeFloat(hitActorDotProduct));
			}
			
		}
	}
}
