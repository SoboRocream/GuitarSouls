// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSGASGameOverWidget.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGSGASGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RestartButton;

	UFUNCTION()
	void OnRestartClicked();
};
