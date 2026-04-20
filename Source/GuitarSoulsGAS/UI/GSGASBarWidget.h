// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASBarWidget : public UGSGASUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateBar(float CurrentValue, float MaxValue);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetRatio(float Percent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> StatBar;
	
	
};
