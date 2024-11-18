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
	dmgText._text->Font.Size = _textSize;
	dmgText._isCrit = crit;
	dmgText._textSpeed = _textSpeed;

	if (crit)
		dmgText._text->Font.Size = _critStartSize;
	

	dmgText._isActive        = true;
	dmgText._currentDuration = 0.0f;
	
	float xSpawn = _spawnAreaSize.X / 2.0f;
	float ySpawn = _spawnAreaSize.Y / 2.0f;

	xSpawn = FMath::RandRange(-xSpawn, xSpawn);
	ySpawn = FMath::RandRange(-ySpawn, ySpawn);
	dmgText._position     = { xSpawn, ySpawn };

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
		_pooledDamageTexts[i]._text->SetFont(_fontInfo);
		_pooledDamageTexts[i]._text->SetRenderOpacity(0.0f);
		_pooledDamageTexts[i]._text->SetJustification(ETextJustify::Center);
		_pooledDamageTexts[i]._text->Font.Size = _textSize;
		_pooledDamageTexts[i]._isActive = false;
		UCanvasPanelSlot* panel = _canvas->AddChildToCanvas(_pooledDamageTexts[i]._text);
		panel->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		panel->SetPosition({ 0.0f, 0.0f });
		panel->SetAlignment({ 0.5f, 0.5f });
	}

}

void UDamageDisplay::TickDamageDisplays(const float deltaTime)
{
	//_nextSpawnDuration -= deltaTime;
	//
	//for (INT32 i = 0; i < _activeDamageTexts.Num(); i++)
	//{
	//	DamageText& dmgText = _pooledDamageTexts[_activeDamageTexts[i]];
	//
	//	if (!dmgText._isActive)
	//		continue;
	//	
	//	
	//	dmgText._position.Y += deltaTime * _textSpeed;
	//	dmgText._text->SetRenderTranslation(dmgText._position);
	//
	//	if (dmgText._currentDuration <= 0.0f)
	//	{
	//		dmgText._isActive = false;
	//		dmgText._inUse    = false;
	//		dmgText._text->SetRenderOpacity(0.0f);
	//		RemoveTextFromActiveList(i);
	//	}
	//	else
	//		dmgText._currentDuration -= deltaTime;	
	//	
	//}
}

void UDamageDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	_nextSpawnDuration -= InDeltaTime;

	for (INT32 i = 0; i < _activeDamageTexts.Num(); i++)
	{
		DamageText& dmgText = _pooledDamageTexts[_activeDamageTexts[i]];

		if (!dmgText._isActive)
			continue;

		if (dmgText._isCrit)
			CritTextMovement(dmgText, InDeltaTime);
		else
			RegularTextMovement(dmgText, InDeltaTime);

		dmgText._text->SetRenderTranslation(dmgText._position);

		if (dmgText._currentDuration >= _textDuration)
		{
			dmgText._isActive = false;
			dmgText._text->SetRenderOpacity(0.0f);
			RemoveTextFromActiveList(i);
		}
		else
			dmgText._currentDuration += InDeltaTime;

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


void UDamageDisplay::CritTextMovement(DamageText& dmgText, const float deltTime)
{
	float alpha = 0.0f;
	if(dmgText._currentDuration != 0.0f)
		alpha = dmgText._currentDuration / _critLerpDuration;

	float size = FMath::Lerp(_critStartSize, _critTextIncrease, FMath::Clamp(alpha, 0.0f, 1.0f));
	dmgText._text->SetRenderScale({ size, size });

	dmgText._position.Y += (dmgText._textSpeed * _critMovement) * deltTime;
}

void UDamageDisplay::RegularTextMovement(DamageText& dmgText, const float deltTime)
{
	if (dmgText._position.X < 0.0f)
		dmgText._position.X += (_textSpeed) * deltTime;
	else
		dmgText._position.X -= (_textSpeed) * deltTime;

	dmgText._position.Y += dmgText._textSpeed * deltTime;

	dmgText._textSpeed += _textGravityStrength * deltTime;
}
