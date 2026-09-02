// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASPlayerHUDWidget : public UGSGASUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	// 화면 프롬프트 표시. 상호작용 안내와 튜토리얼 안내가 같은 채널을 공유한다.
	// Duration <= 0 이면 HidePromptText가 불릴 때까지 유지(상호작용 오버랩 용도),
	// Duration > 0 이면 그 시간 뒤 자동으로 사라진다(튜토리얼 트리거 용도).
	UFUNCTION(BlueprintCallable, Category = "UI|Prompt")
	void ShowPromptText(const FText& InText, float Duration = 0.f);

	UFUNCTION(BlueprintCallable, Category = "UI|Prompt")
	void HidePromptText();

protected:
	// WBP의 Visibility 기본값과 무관하게 프롬프트가 숨겨진 상태로 시작하도록 보장
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASBarWidget> HpBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASBarWidget> StaminaBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASPotionWidget> PotionWidget;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> InteractPromptText;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnPotionCountChanged(const FOnAttributeChangeData& Data);

	// Duration 지정 시 자동 숨김용
	FTimerHandle PromptTimerHandle;
};
