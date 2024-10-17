#pragma once
#include "LingeringFire.h"
#include "ProjectileBaseClass.h"
#include "FireProjectile.generated.h"

class ALingeringFire;
class AAuraHandler;

UCLASS()
class AFireProjectile : public AProjectileBaseClass
{
	GENERATED_BODY()
	
public:
	
	virtual void SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer) override;

	virtual void BeginPlay() override;
	
	virtual void DealDamage(TArray<AActor*> hitCharacter) override;

	void FireExplosion(FVector explodeOrigin);

	void dealFireDamage(AAuraCharacter* Character);

	UFUNCTION()
	virtual void SetOnFire(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent*
		OtherComp, int32 OtherBodyIndexbool ,bool bFromSweep,const FHitResult& SweepResult);
	
	UPROPERTY(EditDefaultsOnly, Category="Explosion Sats") int _explosionDamage = _projectileDamage;

	UPROPERTY(EditAnywhere, Category="Projectile Stats") int biggerOilExplosionThreshold = 3;
	UPROPERTY(EditAnywhere, Category="Projectile Stats") int lingeringFireThreshold		 = 4;

	bool _biggerOilExplosion = false;
	bool _lingeringFire		 = false;

	UPROPERTY(EditDefaultsOnly) TSubclassOf<ALingeringFire> lingeringFire = ALingeringFire::StaticClass();

	
	UPROPERTY() ALingeringFire*	_lingeringFireActor = nullptr;
};
