// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASVictoryWidget.h"
#include "GuitarSoulsGAS.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Save/GSGASPersistenceSubsystem.h"
#include "Save/GSGASRunTimerSubsystem.h"

void UGSGASVictoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGSGASVictoryWidget::OnReturnClicked);
	}

	// 기록 표시 — 보스 사망 시점에 StopRun으로 이미 확정되어 있다.
	// StartRun 없이 전투 맵을 직접 연 경우(PIE) IsRunFinished가 false라 기록 UI를 숨긴다.
	UGSGASRunTimerSubsystem* RunTimer = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGSGASRunTimerSubsystem>() : nullptr;
	const bool bHasRecord = RunTimer && RunTimer->IsRunFinished();
	const int32 Rank = bHasRecord ? RunTimer->GetRankForTime(RunTimer->GetElapsedSeconds()) : 0;
	bRecordEligible = Rank > 0;

	if (TimeText)
	{
		TimeText->SetVisibility(bHasRecord ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		if (bHasRecord)
		{
			TimeText->SetText(UGSGASRunTimerSubsystem::FormatTime(RunTimer->GetElapsedSeconds()));
		}
	}

	if (RankText)
	{
		RankText->SetVisibility(bHasRecord ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		RankText->SetText(bRecordEligible ? FText::Format(INVTEXT("RANK {0}"), Rank) : INVTEXT("OUT OF RANK"));
	}

	if (NameInput)
	{
		NameInput->SetVisibility(bRecordEligible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		NameInput->OnTextChanged.AddDynamic(this, &UGSGASVictoryWidget::OnNameChanged);
		NameInput->OnTextCommitted.AddDynamic(this, &UGSGASVictoryWidget::OnNameCommitted);
	}
	else if (bRecordEligible)
	{
		// 입력칸이 없으면 이름을 받을 수 없으므로 기록 없이 복귀만 허용
		GSGAS_LOG(LogGSGAS, Warning, TEXT("NameInput is not bound — record will not be submitted."));
		bRecordEligible = false;
	}

	if (QuitButton)
	{
		QuitButton->SetIsEnabled(!bRecordEligible);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(bRecordEligible ? NameInput->TakeWidget() : TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UGSGASVictoryWidget::OnReturnClicked()
{
	if (bReturning)
	{
		return;
	}

	if (ReturnLevel.IsNull())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Victory return aborted: ReturnLevel is not set."));
		return;
	}

	UGameInstance* GI = GetGameInstance();

	if (bRecordEligible)
	{
		UGSGASRunTimerSubsystem* RunTimer = GI ? GI->GetSubsystem<UGSGASRunTimerSubsystem>() : nullptr;
		if (!RunTimer || !RunTimer->SubmitRecord(NameInput->GetText().ToString()))
		{
			return;
		}
	}

	bReturning = true;

	// 런 종료 — 체크포인트를 지워야 다음 관람자가 무기·스탯을 물려받지 않는다.
	if (GI)
	{
		if (UGSGASPersistenceSubsystem* Persistence = GI->GetSubsystem<UGSGASPersistenceSubsystem>())
		{
			Persistence->ClearSaveData();
		}
	}

	// UIOnly 상태가 남아있을 수 있으므로 레벨 이동 전에 명시적으로 초기화
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly GameInputMode;
		PC->SetInputMode(GameInputMode);
		PC->bShowMouseCursor = false;
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("Victory return -> %s"), *ReturnLevel.ToString());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, ReturnLevel);
}

void UGSGASVictoryWidget::OnNameChanged(const FText& Text)
{
	const FString Sanitized = SanitizeRecordName(Text.ToString());
	if (!Sanitized.Equals(Text.ToString(), ESearchCase::CaseSensitive))
	{
		NameInput->SetText(FText::FromString(Sanitized));
	}

	if (QuitButton)
	{
		QuitButton->SetIsEnabled(UGSGASRunTimerSubsystem::IsValidRecordName(Sanitized));
	}
}

void UGSGASVictoryWidget::OnNameCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && UGSGASRunTimerSubsystem::IsValidRecordName(Text.ToString()))
	{
		OnReturnClicked();
	}
}

FString UGSGASVictoryWidget::SanitizeRecordName(const FString& Raw)
{
	FString Out;
	for (TCHAR C : Raw.ToUpper())
	{
		if (Out.Len() < 3 && C >= TEXT('A') && C <= TEXT('Z'))
		{
			Out.AppendChar(C);
		}
		else if (Out.Len() >= 3 && Out.Len() < 5 && C >= TEXT('0') && C <= TEXT('9'))
		{
			Out.AppendChar(C);
		}
	}
	return Out;
}
