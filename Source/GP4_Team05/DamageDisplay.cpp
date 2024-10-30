#include "DamageDisplay.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void UDamageDisplay::DisplayDamage(int amount, ElementTypes type, bool crit)
{
	INT32 index = -1;
	for (INT32 i = 0; i < _pooledDamageTexts.Num(); i++)
	{
		if (!_pooledDamageTexts[i]._isActive) {
			index = i;
			break;
		}
	}

	if (index == -1)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Damage is Added!"));
	DamageText& dmgText = _pooledDamageTexts[index];

	FString damage = "";
	damage.AppendInt(amount);
	FText text = FText(FText::FromString(damage));
	dmgText._text->SetText(text);
	FSlateColor color(FColor::White);

	switch (type)
	{
	case PHYSICAL:
		color = _physicalColor;
		break;
	case FIRE:
		color = _fireColor;
		break;
	case LIGHTNING:
		color = _lightningColor;
		break;
	case BLOOD:
		color = _bloodColor;
		break;
	}

	dmgText._text->SetColorAndOpacity(color);
	dmgText._text->SetRenderOpacity(1.0f);
	if (crit)
		dmgText._text->Font.Size *= 1.5f;
	else
		dmgText._text->Font.Size = _textSize;

	dmgText._isActive        = true;
	dmgText._currentDuration = _textDuration;
	dmgText._position        = { 0.0f, 0.0f };

	_activeDamageTexts.Add(index);
}

void UDamageDisplay::SetUpDamageDipslay()
{
	if (!_canvas)
		return; 

	WidgetTree->RootWidget = _canvas;
	for (INT32 i = 0; i < _textPoolSize; i++)
	{
		_pooledDamageTexts.Add(DamageText());
		FString index = "";
		index.AppendInt(i);
		_pooledDamageTexts[i]._text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString("Damage Text " + index)));
		//_pooledDamageTexts[i]._text->SetText(FText(FText::FromString("TEXT HERE!")));
		_pooledDamageTexts[i]._text->SetJustification(ETextJustify::Center);
		_pooledDamageTexts[i]._text->SetRenderOpacity(0.0f);
		_pooledDamageTexts[i]._text->Font.Size = _textSize;
		UCanvasPanelSlot* panel = _canvas->AddChildToCanvas(_pooledDamageTexts[i]._text);
		panel->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		panel->SetPosition({ 0.0f, 0.0f });
		panel->SetAlignment({ 0.5f, 0.5f });
		_pooledDamageTexts[i]._slot = panel;
		_pooledDamageTexts[i]._text->SetRenderTransform(_canvas->GetRenderTransform());
	}
}

void UDamageDisplay::TickDamageDisplays(const float deltaTime)
{
	for (INT32 i = 0; i < _activeDamageTexts.Num(); i++)
	{
		DamageText& dmgText = _pooledDamageTexts[_activeDamageTexts[i]];
		if (!dmgText._isActive)
			continue;

		//dmgText._text->SetRenderTransform(_canvas->GetRenderTransform());
		dmgText._position.Y += deltaTime * _textSpeed;
		dmgText._slot->SetPosition(dmgText._position);

		if (dmgText._currentDuration <= 0.0f)
		{
			dmgText._isActive = false;
			dmgText._text->SetRenderOpacity(0.0f);
			RemoveTextFromActiveList(i);
		}
		else
			dmgText._currentDuration -= deltaTime;	
	}
}

void UDamageDisplay::RemoveTextFromActiveList(INT32 index)
{

	if (_activeDamageTexts.Num() > 1)
	{
		_activeDamageTexts[index] = _activeDamageTexts[_activeDamageTexts.Num() - 1];
		_activeDamageTexts.Pop();
	}
	else
		_activeDamageTexts.Pop();
}
