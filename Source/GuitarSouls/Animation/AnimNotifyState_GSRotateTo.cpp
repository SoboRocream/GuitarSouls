// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSRotateTo.h"

#include "Component/GSRotationComponent.h"

void UAnimNotifyState_GSRotateTo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* Owner = MeshComp->GetOwner())
	{
		if (UGSRotationComponent* RotationComponent = Owner->GetComponentByClass<UGSRotationComponent>())
		{
			RotationComponent->ToggleShoudRotate(true);
		}
	}
	
}

void UAnimNotifyState_GSRotateTo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (const AActor* Owner = MeshComp->GetOwner())
	{
		if (UGSRotationComponent* RotationComponent = Owner->GetComponentByClass<UGSRotationComponent>())
		{
			RotationComponent->ToggleShoudRotate(false);
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
