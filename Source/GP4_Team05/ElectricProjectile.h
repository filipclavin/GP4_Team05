#pragma once
#include "ProjectileBaseClass.h"
#include "ElectricProjectile.generated.h"


UCLASS()
class AElectricProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
public:
	void SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer) override;
	
	void HitEnemies(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent*
		OtherComp,int32 OtherBodyIndexbool,bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, Category="Projectile Stats") int enemyTypeFilterThreshold   = 1;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int enemyHitOverspillThreshold = 2;

	bool _filterEnemyType = false;
	bool _overspillHits   = false;

	UFUNCTION(BlueprintImplementableEvent) void LightningHitEvent(const TMap<AActor*, FLightningStruct>& electricityForks);

	const AActor* _initialHit = nullptr;
};
