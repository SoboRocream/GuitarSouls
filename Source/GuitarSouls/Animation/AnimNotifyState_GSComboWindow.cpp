// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSComboWindow.h"

#include "Character/GSCharacterPlayer.h"

UAnimNotifyState_GSComboWindow::UAnimNotifyState_GSComboWindow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UAnimNotifyState_GSComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (AGSCharacterPlayer* Character = Cast<AGSCharacterPlayer>(MeshComp->GetOwner()))
	{
		Character->EnableComboWindow();
	}
}

void UAnimNotifyState_GSComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AGSCharacterPlayer* Character = Cast<AGSCharacterPlayer>(MeshComp->GetOwner()))
	{
		Character->DisableComboWindow();
	}
}
