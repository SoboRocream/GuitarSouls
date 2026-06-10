// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSGASVictoryWidget.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGSGASVictoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton;

	UFUNCTION()
	void OnQuitClicked();
};
