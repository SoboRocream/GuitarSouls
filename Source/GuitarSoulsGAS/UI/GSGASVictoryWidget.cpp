// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASVictoryWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UGSGASVictoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGSGASVictoryWidget::OnQuitClicked);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UGSGASVictoryWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
