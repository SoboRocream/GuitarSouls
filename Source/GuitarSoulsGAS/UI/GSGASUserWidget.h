// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "GSGASUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASUserWidget : public UUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	void SetASC(class UAbilitySystemComponent* InASC);

protected:
	virtual void InitializeWidget(UAbilitySystemComponent* InASC) {}

	FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponent() const { return ASC.Get(); }

private:
	UPROPERTY()
	TWeakObjectPtr<class UAbilitySystemComponent> ASC;
};
