// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "GSGASVictoryWidget.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGSGASVictoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 이름은 WBP 바인딩 유지용. 동작은 종료가 아니라 기록 제출 후 ReturnLevel 복귀다(전시 연속 운영).
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton;

	// 클리어 시간 (mm:ss.cc)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> TimeText;

	// 순위 / 순위권 밖
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> RankText;

	// 이니셜 3글자 + 숫자 2자리. 순위권 안일 때만 표시되고, 유효할 때까지 QuitButton이 비활성.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UEditableTextBox> NameInput;

	// 런 종료 후 돌아갈 맵 — 에디터에서 .umap 드롭다운으로 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Victory")
	TSoftObjectPtr<UWorld> ReturnLevel;

	UFUNCTION()
	void OnReturnClicked();

	UFUNCTION()
	void OnNameChanged(const FText& Text);

	UFUNCTION()
	void OnNameCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	// 입력을 대문자화하고 자리별로 거른다: 앞 3자리는 A~Z, 뒤 2자리는 0~9
	static FString SanitizeRecordName(const FString& Raw);

	// 이번 기록이 순위권 안이라 이름을 받아야 하는가
	bool bRecordEligible = false;

	// 버튼 연타·엔터 중복 방지
	bool bReturning = false;
};
