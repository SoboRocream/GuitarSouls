// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/GSGASAnimNotifyState_LooseTag.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void UGSGASAnimNotifyState_LooseTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!Tag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UGSGASAnimNotifyState_LooseTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!Tag.IsValid()) return;

	if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

FString UGSGASAnimNotifyState_LooseTag::GetNotifyName_Implementation() const
{
	// 타임라인에 태그 이름이 바로 보이도록
	return Tag.IsValid() ? Tag.GetTagName().ToString() : TEXT("GAS Loose Tag (None)");
}

UAbilitySystemComponent* UGSGASAnimNotifyState_LooseTag::GetASC(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp) return nullptr;

	const AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return nullptr;

	const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (!ASCInterface) return nullptr;

	return ASCInterface->GetAbilitySystemComponent();
}
