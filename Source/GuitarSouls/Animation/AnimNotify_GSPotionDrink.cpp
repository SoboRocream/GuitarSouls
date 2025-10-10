// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_GSPotionDrink.h"

#include "Component/GSPotionInventoryComponent.h"

void UAnimNotify_GSPotionDrink::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (UGSPotionInventoryComponent* PotionInventoryComponent = OwnerActor->GetComponentByClass<UGSPotionInventoryComponent>())
		{
			PotionInventoryComponent->DrinkPotion();
		}
	}
}
