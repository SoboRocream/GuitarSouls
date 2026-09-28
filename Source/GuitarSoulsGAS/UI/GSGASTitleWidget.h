// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSGASTitleWidget.generated.h"

// 시작 화면. 메인 패널(Start / Leaderboard)과 순위표 패널을 PanelSwitcher로 전환한다.
// PanelSwitcher 자식 순서: 0 = 메인, 1 = 순위표
UCLASS()
class GUITARSOULSGAS_API UGSGASTitleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> PanelSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> LeaderboardButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton;

	// 순위표 전체를 줄바꿈 텍스트로 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> EntriesText;

	// 런 시작 맵 — 에디터에서 .umap 드롭다운으로 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Title")
	TSoftObjectPtr<UWorld> StartLevel;

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnLeaderboardClicked();

	UFUNCTION()
	void OnBackClicked();
};
