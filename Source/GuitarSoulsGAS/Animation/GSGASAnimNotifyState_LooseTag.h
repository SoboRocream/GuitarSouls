// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "GSGASAnimNotifyState_LooseTag.generated.h"

/**
 * 에디터에서 지정한 GameplayTag를 구간 동안 AddLooseGameplayTag / RemoveLooseGameplayTag 하는 범용 NotifyState.
 * ComboWindow, Invincible, Parrying 등 태그 On/Off가 필요한 모든 구간에 재사용한다.
 */
UCLASS(meta = (DisplayName = "GAS Loose Tag"))
class GUITARSOULSGAS_API UGSGASAnimNotifyState_LooseTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// 에디터에서 구간에 부여할 태그를 직접 지정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag Tag;

private:
	UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
