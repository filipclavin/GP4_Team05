#pragma once
#include "ProjectileBaseClass.h"
#include "FireProjectile.generated.h"

class AAuraHandler;

UCLASS()
class AFireProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
	
public:
	AAuraHandler* _handler = nullptr;

	virtual void BeginPlay() override;
	
	virtual void DealDamage(TArray<AActor*> hitCharacter) override;

	void FireExplosion(FVector explodeOrigin);

	void dealFireDamage(AAuraCharacter* Character);

	UPROPERTY(EditDefaultsOnly, Category="Explosion Sats") int _explosionRange  = 300;
	UPROPERTY(EditDefaultsOnly, Category="Explosion Sats") int _explosionDamage = _projectileDamage;
};
