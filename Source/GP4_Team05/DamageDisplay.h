#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraDataInclude.h"
#include "DamageDisplay.generated.h"

class UBorder;
class UTextBlock;
class UCanvasPanel;
class UCanvasPanelSlot;

UCLASS(Abstract)
class GP4_TEAM05_API UDamageDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void DisplayDamage(int amount, ElementTypes type, bool crit);

	UFUNCTION(BlueprintCallable) void SetUpDamageDipslay();
	UFUNCTION(BlueprintCallable) void TickDamageDisplays(const float deltaTime);

protected:
	void RemoveTextFromActiveList(INT32 index);

	struct DamageText {
		UTextBlock* _text			 = nullptr;
		FVector2D	_position        = { 0.0f, 0.0f };
		bool		_isActive        = false;
		bool		_inUse           = false;
		float		_currentDuration = 0.0f;
	};

	TArray<DamageText>  _pooledDamageTexts;
	TArray<INT32>       _activeDamageTexts;

	UPROPERTY(EditAnywhere, meta = (BindWidget)) UCanvasPanel* _canvas    = nullptr;

	UPROPERTY(EditAnywhere) FSlateFontInfo _fontInfo;
	UPROPERTY(EditAnywhere) FColor         _physicalColor;
	UPROPERTY(EditAnywhere) FColor         _fireColor;
	UPROPERTY(EditAnywhere) FColor         _lightningColor;
	UPROPERTY(EditAnywhere) FColor         _bloodColor;

	UPROPERTY(EditAnywhere) FVector2D _spawnAreaSize	 = { 200.0f, 20.0f };
	UPROPERTY(EditAnywhere) int		  _textPoolSize      = 10;
	UPROPERTY(EditAnywhere) float	  _textSpawnSpeed    = 0.1f;
	UPROPERTY(EditAnywhere) float	  _textDuration      = 2.0f;
	UPROPERTY(EditAnywhere) float	  _textSpeed         = 1.0f;
	UPROPERTY(EditAnywhere) float	  _textSize          = 40.0f;
	UPROPERTY(EditAnywhere) float	  _critTextIncrease  = 2.0f;

	float _nextSpawnDuration = 0.0f;
};
