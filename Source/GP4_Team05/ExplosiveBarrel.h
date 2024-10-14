#pragma once
#include "AuraCharacter.h"
#include "ExplosiveBarrel.generated.h"

class UBoxComponent;

UCLASS()
class AExplosiveBarrel : public AAuraCharacter
{
	GENERATED_BODY()

	AExplosiveBarrel();
public:
	virtual void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere) UBoxComponent* _oilCollider = nullptr;

	UPROPERTY(VisibleAnywhere) float _burningDuration = 5;
	
	void Despawn();

	bool _oilSpilled = false;
	bool _onFire	 = false;
	
	void SpillOil();

	UFUNCTION(BlueprintCallable) void SetFire();

	UFUNCTION(BlueprintCallable) void EndFire();

	void SpreadFire();
	void Explode   ();

	UPROPERTY(EditDefaultsOnly, Category="explosion") int explosionRadius = 300;
	UPROPERTY(EditDefaultsOnly, Category="explosion") int explosionDamage = 20;


	UFUNCTION(BlueprintImplementableEvent) void SpillOilEvent();
	UFUNCTION(BlueprintImplementableEvent) void SetFireEvent ();
};
