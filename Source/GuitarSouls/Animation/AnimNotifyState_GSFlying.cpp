// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSFlying.h"

#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_GSFlying::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerActor->GetComponentByClass<UCharacterMovementComponent>())
		{
			MovementComponent->SetMovementMode(MOVE_Flying);
		}
	}
}

void UAnimNotifyState_GSFlying::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerActor->GetComponentByClass<UCharacterMovementComponent>())
		{
			MovementComponent->SetMovementMode(MOVE_Walking);
		}
	}
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
