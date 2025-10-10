// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSIFrames.h"

#include "Interface/GSCombatInterface.h"

void UAnimNotifyState_GSIFrames::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(OwnerActor))
	{
		CombatInterface->ToggleIFrames(true);
	}
}

void UAnimNotifyState_GSIFrames::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(OwnerActor))
	{
		CombatInterface->ToggleIFrames(false);
	}
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
