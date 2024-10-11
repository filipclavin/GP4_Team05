#pragma once
#include "ProjectileBaseClass.h"
#include "ElectricProjectile.generated.h"

UCLASS()
class AElectricProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
public:
	void HitEnemies(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent*
		OtherComp,int32 OtherBodyIndexbool,bool bFromSweep, const FHitResult& SweepResult) override;
};
