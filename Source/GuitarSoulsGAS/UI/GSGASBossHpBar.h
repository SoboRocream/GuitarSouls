// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASBossHpBar.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASBossHpBar : public UGSGASUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void InitializeWidget(UAbilitySystemComponent* InASC) override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> HealthBarWidget;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
};
