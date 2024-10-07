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
#include "Kismet/KismetMathLibrary.h"


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
		Input->BindAction(_attack, ETriggerEvent::Started, this, &APlayerCharacter::MeleeAction);
		Input->BindAction(_attack, ETriggerEvent::Started, this, &APlayerCharacter::RangeAttackAction);
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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	if (CurrentlyDashing)
	{
		SetActorLocation(FMath::Lerp(_dashStartLocation, _dashTargetLocation, easeInOutQuint(GetWorld()->GetTimerManager().GetTimerElapsed(_dashHandle)/_dashDuration)));
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



void APlayerCharacter::MeleeAction(const FInputActionValue& Value)
{
	if (_aiming) {return;}
	if (_meleeCooldown > _meleeCooldownTimer){GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "cooldown"); return;}
	

	_meleeCooldownTimer = 0;

	MeleeAttackEvent();
	
	TArray<AActor*> HitActors;
	_meleeHitbox->GetOverlappingActors(HitActors);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "melee");
	
	for (AActor* HitActor : HitActors)
	{
		if (HitActor != this)
		{
			if (HitActor->IsA<AAuraCharacter>())
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, HitActor->GetName() + " hit");
				Cast<AAuraCharacter>(HitActor)->QueueDamage(_meleeDamage, PHYSICAL);
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
	
	if (_projectilePrefab)
	{
		FVector AimStart;
		FRotator CamRot;
		GetController<APlayerController>()->GetPlayerViewPoint(AimStart, CamRot);
        
		FVector CamForward = CamRot.Quaternion().GetForwardVector();
        		
		FHitResult AimHit;
		GetWorld()->LineTraceSingleByChannel(AimHit,AimStart, AimStart + CamForward*10000.f,ECC_Visibility);
		
		FVector AimPoint = AimHit.bBlockingHit ? AimHit.Location: AimHit.TraceEnd;

		FVector BulletOrg = GetActorLocation() + GetActorForwardVector()*200.f;
		FVector BulletDir = AimPoint - BulletOrg;
		
		GetWorld()->SpawnActor<AActor>(_projectilePrefab,GetActorLocation(),UKismetMathLibrary::MakeRotFromX(BulletDir));
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

	
	TArray<FHitResult> dashHits;
	FHitResult		   solidHit;

	FVector dashDirection = _inputVector.Y*GetActorForwardVector()+_inputVector.X*GetActorRightVector(); 

	if (dashDirection == FVector::Zero())
	{
		dashDirection = GetActorForwardVector();
	}
	
	dashDirection.Normalize();

	GetWorld()->SweepMultiByObjectType(dashHits, GetActorLocation(), GetActorLocation()+dashDirection * _dashLength,
		GetActorRotation().Quaternion(),ECC_Pawn, FCollisionShape::MakeSphere(100.f));
	GetWorld()->LineTraceSingleByObjectType(solidHit, GetActorLocation(), GetActorLocation()+dashDirection * 1000.f, ECC_WorldStatic);

	FVector rightOfDash = dashDirection.Cross(FVector::UpVector);

	//in order to prevent actors from getting hit multiple time, check a hashset if the actor has been hit before -Gustav
	TSet<AActor*> hitActors;
	
	for(FHitResult DashHit : dashHits)
	{
		
		AActor* HitActor = DashHit.GetActor();


		if (!hitActors.Contains(HitActor))
		{
			if (HitActor->IsA<AAuraCharacter>() && HitActor != this)
			{
				//Cast<AAuraCharacter>(HitActor)->GetCharacterMovement()->AddImpulse(DashHit.Normal*-300000);
				//HitActor->SetActorLocation(HitActor->GetActorLocation() + DashHit.Normal*_dashKnockBack*-1);
            
				FVector knockbackDirection = rand() % 2 ? rightOfDash:-rightOfDash;
            			
				HitActor->SetActorLocation(HitActor->GetActorLocation() + knockbackDirection*_dashKnockBack);
				Cast<AAuraCharacter>(HitActor)->QueueDamage(_dashDamage, ElementTypes::PHYSICAL);
			}
			hitActors.Add(HitActor);
		}
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::FromInt(hitActors.Num()));
	//SetActorLocation((dashDirection * 1000.f)+ GetActorLocation());

	CurrentlyDashing = true;
	_dashTargetLocation = solidHit.bBlockingHit ? solidHit.ImpactPoint : solidHit.TraceEnd;
	//_dashTargetLocation = dashDirection * _dashLength + GetActorLocation();
	_dashStartLocation  = GetActorLocation();
	
	GetWorld()->GetTimerManager().SetTimer(_dashHandle, FTimerDelegate::CreateLambda([this] (){CurrentlyDashing = false;}), _dashDuration, false);
}