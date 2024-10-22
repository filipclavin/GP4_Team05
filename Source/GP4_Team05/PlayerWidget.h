#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerWidget.generated.h"

class AAuraCharacter;
class UCharacterStats;
class UAura;

class UImage;
class UTextBlock;
class UVerticalBox;
class UHorizontalBox;
class UProgressBar;

UCLASS(Abstract)
class GP4_TEAM05_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) void SetPlayer(AAuraCharacter* player);
	UFUNCTION(BlueprintCallable) void InitializeAuraTracker();

	UFUNCTION(BlueprintCallable) void UpdateHUD();
	UFUNCTION(BlueprintCallable) void UpdateHealth();

protected:

	struct AuraTrackerData {
		UVerticalBox* _verticalBox = nullptr;
		UTextBlock*   _text		   = nullptr;
		UImage*		  _image	   = nullptr;
	};

	void SetupIconsAndTexts(UHorizontalBox* box, TArray<AuraTrackerData>& tracker, FString type);

	float Precision(float f, int places);
	int   RountToInt(float amount);

	void UpdateAuras(const TArray<UAura*>& list, TArray<AuraTrackerData>& trackerData);

	UPROPERTY(EditAnywhere) int       _maxTrackedAuras = 10;
	UPROPERTY(EditAnywhere) float     _imageSizeX      = 2.0f;
	UPROPERTY(EditAnywhere) float     _imageSizeY      = 2.0f;
	UPROPERTY(EditAnywhere) float     _fontSize        = 20.0f;
	UPROPERTY(EditAnywhere) FVector2D _textPosition    = { 0.0f, 0.0f };

	UPROPERTY(BlueprintReadWrite) AAuraCharacter*  _player;
	UPROPERTY(BlueprintReadWrite) UCharacterStats* _playerStats;

	UPROPERTY(EditAnywhere, meta = (BindWidget)) UProgressBar* _healthBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UTextBlock*   _healthText;

	UPROPERTY(EditAnywhere, meta = (BindWidget)) UHorizontalBox* _buffsBox;
	UPROPERTY(EditAnywhere, meta = (BindWidget)) UHorizontalBox* _debuffsBox;


	TArray<AuraTrackerData> _buffsTracker;
	TArray<AuraTrackerData> _debuffsTracker;
};
