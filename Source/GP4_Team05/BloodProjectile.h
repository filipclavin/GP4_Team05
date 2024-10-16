#pragma once
#include "ProjectileBaseClass.h"
#include "BloodProjectile.generated.h"

UCLASS()
class ABloodProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
public:
	virtual void HitEnemies(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void DealDamage(TArray<AActor*> hitCharacter) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer) override;

	UPROPERTY(EditAnywhere, Category="Projectile Stats") float abilityDuration = 0.4;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") float AttackConeAngle = 0.3;

	TSet<AActor*> hitActors;

	float _durationTimer = 0;
};
