// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/GSGASAnimNotifyState_ComboWindow.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Tags/GSGASGameplayTags.h"

void UGSGASAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* Owner = MeshComp->GetOwner())
	{
		if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner))
		{
			if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
			{
				ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_ComboWindow);
			}
		}
	}
}

void UGSGASAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (const AActor* Owner = MeshComp->GetOwner())
	{
		if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner))
		{
			if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
			{
				ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_ComboWindow);
			}
		}
	}
}
