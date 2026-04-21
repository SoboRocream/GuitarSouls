// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASPlayerHUDWidget : public UGSGASUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASBarWidget> HpBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASBarWidget> StaminaBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSPotionWidget> PotionWidget;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> InteractPromptText;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnPotionCountChanged(const FOnAttributeChangeData& Data);
	
};
