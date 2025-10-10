// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_GSPotionSpawn.h"

#include "Component/GSPotionInventoryComponent.h"

void UAnimNotifyState_GSPotionSpawn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UGSPotionInventoryComponent* PotionInventoryComponent = OwnerActor->GetComponentByClass<UGSPotionInventoryComponent>())
		{
			PotionInventoryComponent->SpawnPotion();
		}
	}
}

void UAnimNotifyState_GSPotionSpawn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UGSPotionInventoryComponent* PotionInventoryComponent = OwnerActor->GetComponentByClass<UGSPotionInventoryComponent>())
		{
			PotionInventoryComponent->DespawnPotion();
		}
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
