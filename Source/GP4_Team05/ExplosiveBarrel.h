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
	void Explode   (bool biggerExplode = false);

	UPROPERTY(EditDefaultsOnly, Category="explosion") int explosionRadius		= 300;
	//to be used when the fire ball is upgraded
	UPROPERTY(EditDefaultsOnly, Category="explosion") int biggerExplosionRadius = 600;
	UPROPERTY(EditDefaultsOnly, Category="explosion") int explosionDamage		= 20;

	//chance of the barrel despawning on begin play in percent
	UPROPERTY(EditDefaultsOnly, Category="barrel") int despawnChance = 50;

	
	UFUNCTION(BlueprintImplementableEvent) void SpillOilEvent();
	UFUNCTION(BlueprintImplementableEvent) void SetFireEvent ();
	UFUNCTION(BlueprintImplementableEvent) void ExplodeEvent (int ExplodeRadius);
};
