// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSGASBarWidget.h"
#include "AbilitySystemComponent.h"
#include "GSGASEnemyHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASEnemyHpBarWidget : public UGSGASBarWidget
{
	GENERATED_BODY()

protected:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	void UpdateHpBar();
	
protected:
	float CurrentHealth = 0.0f;
	float CurrentMaxHealth = 0.1f;
};
