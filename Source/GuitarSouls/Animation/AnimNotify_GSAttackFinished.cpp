// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_GSAttackFinished.h"

#include "Character/GSCharacterPlayer.h"

UAnimNotify_GSAttackFinished::UAnimNotify_GSAttackFinished(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAnimNotify_GSAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (AGSCharacterPlayer* Character = Cast<AGSCharacterPlayer>(MeshComp->GetOwner()))
	{
		Character->AttackFinished(ComboResetDelay);
	}
}
