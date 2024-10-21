#pragma once
#include "ProjectileBaseClass.h"
#include "BloodProjectile.generated.h"


UCLASS()
class ABloodProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
public:
	ABloodProjectile();
	
	virtual void HitEnemies(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void DealDamage(TArray<AActor*> hitCharacter) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer) override;

	UPROPERTY(EditAnywhere, Category="Projectile Stats") float abilityDuration = 0.4;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") float AttackConeAngle = 0.3;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int   ChaosAddPerHit  = 100;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int   healPerHit	    = 10;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int   AddArmorThreshold = 3;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int   GainHealthThreshold = 4;
	UPROPERTY()TSet<AActor*> hitActors;

	bool addArmor = false;
	bool healPlayer = false;
	
	float _durationTimer = 0;
	int	  _enemiesHit	 = 0;

	UPROPERTY(VisibleAnywhere)UStaticMeshComponent* coneMesh = nullptr;
	UPROPERTY(VisibleAnywhere)USceneComponent*		coneRoot = nullptr;
	
	UFUNCTION(BlueprintImplementableEvent) void bloodHitEvent(const TArray<AActor*>& hitEnemies);
};
