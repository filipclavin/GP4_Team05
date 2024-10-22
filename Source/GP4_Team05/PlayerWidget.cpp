#include "PlayerWidget.h"

#include <string>

#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

#include "Aura.h"
#include "AuraCharacter.h"

void UPlayerWidget::SetPlayer(AAuraCharacter* player)
{
	if (player) {
		_player		 = player;
		_playerStats = player->GetStats();
	}
}

void UPlayerWidget::InitializeAuraTracker()
{	
	SetupIconsAndTexts(_buffsBox,   _buffsTracker,   "Buffs");
	SetupIconsAndTexts(_debuffsBox, _debuffsTracker, "Debuffs");
}

void UPlayerWidget::UpdateHUD()
{
	if (!_player)
		return;

	UpdateHealth();

	UpdateAuras(_player->GetAffectedBuffs(),   _buffsTracker);
	UpdateAuras(_player->GetAffectedDebuffs(), _debuffsTracker);
}

void UPlayerWidget::UpdateHealth()
{
	//at least one value in the division needs to be float in order to prevent the result from being an int
	float currentHealth = _playerStats->_currentHealth;
	float maxHealth		= _playerStats->_maxHealth;
	float targetPercent = (currentHealth > 0) ? currentHealth / maxHealth : 0.0f;

	// Adjust InterpSpeed to control how fast the bar fills or shrinks
	const float InterpSpeed = 4.0f;
	float currentBarPercent = _healthBar->Percent;
	float newBarPercent = FMath::FInterpTo(currentBarPercent, targetPercent, GetWorld()->DeltaTimeSeconds, InterpSpeed);

	_healthBar->SetPercent(newBarPercent);
	
	FString toInt = "";
	toInt.AppendInt(_playerStats->_currentHealth);
	toInt += " / ";
	toInt.AppendInt(_playerStats->_maxHealth);
	_healthText->SetText(FText::FromString(toInt));
}

void UPlayerWidget::SetupIconsAndTexts(UHorizontalBox* box, TArray<AuraTrackerData>& tracker, FString type)
{
	for (size_t i = 0; i < _maxTrackedAuras; i++)
	{
		tracker.Add(AuraTrackerData());

		// Widgets need to have unique names otherwise they all point to the same object
		FString index = "(" + FString::FromInt(i) + ")";
		AuraTrackerData& data = tracker[i];
		data._verticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(),  FName(FString(type + "_Tracker_" + index)));
		box->AddChildToHorizontalBox(data._verticalBox);

		data._text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(type + "Countdown" + index)));
		data._text->SetRenderTranslation(_textPosition);
		data._text->Font.Size = _fontSize;
		data._text->SetJustification(ETextJustify::Center);
		data._text->SetText(FText::FromString("0.0"));

		data._image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(FString(type + "_Icon_" + index)));
		data._image->SetRenderScale({ _imageSizeX, _imageSizeY });

		data._verticalBox->AddChildToVerticalBox(data._text);
		data._verticalBox->AddChildToVerticalBox(data._image);
	
		// Only want to render the text & icon when a buff is detected.
		data._verticalBox->SetRenderOpacity(0.0f);

		UHorizontalBoxSlot* slot = Cast<UHorizontalBoxSlot>(box->GetSlots()[i]);
		slot->SetPadding(FMargin(15.0f, 0.0f));
	}
}

// Want to move this to a helper class
float UPlayerWidget::Precision(float f, int places)
{
	float n = powf(1.0f, places);
	return roundf((f * n) / n);
}

int UPlayerWidget::RountToInt(float amount)
{
	amount = amount + 0.5f - (amount < 0);
	return int(amount);
}

void UPlayerWidget::UpdateAuras(const TArray<UAura*>& list, TArray<AuraTrackerData>& trackerData)
{
	// We don't want to run into exception when going through aura list. 
	const INT32 num  = list.Num() < _maxTrackedAuras ? list.Num() : _maxTrackedAuras;

	for (int i = 0; i < _maxTrackedAuras; i++)
	{
		if (i < num)
		{
			// Only display buff Icon if it the Aura has one
			if (list[i]->GetIcon()) {
				AuraTrackerData& data = trackerData[i];
				data._verticalBox->SetRenderOpacity(1.0f);

				FString duration = "";
				duration.AppendInt(RountToInt(list[i]->GetDuration()));//Precision(list[i]->GetDuration(), 2));
				data._text->SetText(FText::FromString(duration));

				data._image->SetBrushFromTexture(list[i]->GetIcon(), false);
			}
		}
		else
		{
			if(trackerData[i]._verticalBox->GetRenderOpacity() > 0.0f)
				trackerData[i]._verticalBox->SetRenderOpacity(0.0f);
		}
	}
}
