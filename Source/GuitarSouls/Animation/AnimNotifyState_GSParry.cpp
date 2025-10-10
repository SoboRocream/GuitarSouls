// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSParry.h"

#include "Character/GSCharacterStateComponent.h"

void UAnimNotifyState_GSParry::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UGSCharacterStateComponent* StateComponent = OwnerActor->GetComponentByClass<UGSCharacterStateComponent>())
		{
			StateComponent->SetState(GSGameplayTags::Character_State_Parrying);
		}
	}
}

void UAnimNotifyState_GSParry::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UGSCharacterStateComponent* StateComponent = OwnerActor->GetComponentByClass<UGSCharacterStateComponent>())
		{
			StateComponent->ClearState();
		}
	}
}
