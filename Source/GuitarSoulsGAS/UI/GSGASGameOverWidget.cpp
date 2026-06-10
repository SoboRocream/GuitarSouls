// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASGameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGSGASGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGSGASGameOverWidget::OnRestartClicked);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UGSGASGameOverWidget::OnRestartClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// UIOnly 상태가 PlayerController에 남아있을 수 있으므로 재시작 전에 명시적으로 초기화
	FInputModeGameOnly GameInputMode;
	PC->SetInputMode(GameInputMode);
	PC->bShowMouseCursor = false;

	PC->RestartLevel();
}
