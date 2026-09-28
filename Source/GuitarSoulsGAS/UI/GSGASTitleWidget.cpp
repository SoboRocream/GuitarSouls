// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASTitleWidget.h"
#include "GuitarSoulsGAS.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Save/GSGASPersistenceSubsystem.h"
#include "Save/GSGASRunTimerSubsystem.h"

void UGSGASTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UGSGASTitleWidget::OnStartClicked);
	}
	if (LeaderboardButton)
	{
		LeaderboardButton->OnClicked.AddDynamic(this, &UGSGASTitleWidget::OnLeaderboardClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UGSGASTitleWidget::OnBackClicked);
	}
	if (PanelSwitcher)
	{
		PanelSwitcher->SetActiveWidgetIndex(0);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UGSGASTitleWidget::OnStartClicked()
{
	if (StartLevel.IsNull())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Run start aborted: StartLevel is not set."));
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		// 이전 런의 체크포인트가 남아 있으면 무기·스탯을 물려받으므로 새 런 시작 시 비운다.
		if (UGSGASPersistenceSubsystem* Persistence = GI->GetSubsystem<UGSGASPersistenceSubsystem>())
		{
			Persistence->ClearSaveData();
		}
		if (UGSGASRunTimerSubsystem* RunTimer = GI->GetSubsystem<UGSGASRunTimerSubsystem>())
		{
			RunTimer->StartRun();
		}
	}

	// UIOnly 상태가 남아있을 수 있으므로 레벨 이동 전에 명시적으로 초기화
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly GameInputMode;
		PC->SetInputMode(GameInputMode);
		PC->bShowMouseCursor = false;
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("Run start -> %s"), *StartLevel.ToString());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, StartLevel);
}

void UGSGASTitleWidget::OnLeaderboardClicked()
{
	if (EntriesText)
	{
		TArray<FGSGASLeaderboardEntry> Entries;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGSGASRunTimerSubsystem* RunTimer = GI->GetSubsystem<UGSGASRunTimerSubsystem>())
			{
				Entries = RunTimer->GetEntries();
			}
		}

		FString Lines;
		for (int32 i = 0; i < Entries.Num(); ++i)
		{
			Lines += FString::Printf(TEXT("%2d.  %s   %s\n"), i + 1, *Entries[i].Name,
				*UGSGASRunTimerSubsystem::FormatTime(Entries[i].TimeSeconds).ToString());
		}
		EntriesText->SetText(Entries.Num() > 0 ? FText::FromString(Lines) : INVTEXT("No records yet."));
	}

	if (PanelSwitcher)
	{
		PanelSwitcher->SetActiveWidgetIndex(1);
	}
}

void UGSGASTitleWidget::OnBackClicked()
{
	if (PanelSwitcher)
	{
		PanelSwitcher->SetActiveWidgetIndex(0);
	}
}
