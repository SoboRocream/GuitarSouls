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
protected:
	virtual void InitializeWidget(UAbilitySystemComponent* InASC) override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> HpBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> StaminaBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSPotionWidget> PotionWidget;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnPotionCountChanged(const FOnAttributeChangeData& Data);
	
};
