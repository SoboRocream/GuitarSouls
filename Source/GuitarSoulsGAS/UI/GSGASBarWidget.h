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
	
public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;
	virtual void SetRatio(float Percent);
	void SetColor(FLinearColor InColor);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> StatBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor DefaultFillColor = FLinearColor::White;
	
};
