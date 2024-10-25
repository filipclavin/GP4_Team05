#pragma once
#include "LingeringFire.generated.h"


class USphereComponent;

class AAuraHandler;

UCLASS()
class ALingeringFire : public AActor
{
	GENERATED_BODY()
public:
	ALingeringFire();
	
	UPROPERTY() AAuraHandler* _handler = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* _fireCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="fire stats") float _fireDuration = 4;
	UPROPERTY(EditDefaultsOnly, Category="fire stats") int	 _fireDamage   = 5;

	UFUNCTION(BlueprintCallable) void SpawnFire(int fireRadius);

	void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void DespawnFire();
};
