// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSPotionInventoryComponent.h"

#include "GSAttributeComponent.h"
#include "GameFramework/Character.h"
#include "Item/GSPotion.h"

UGSPotionInventoryComponent::UGSPotionInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGSPotionInventoryComponent::DrinkPotion()
{
	if(PotionQuantity <= 0)
	{
		return;
	}

	if (UGSAttributeComponent* AttributeComponent = GetOwner()->GetComponentByClass<UGSAttributeComponent>())
	{
		PotionQuantity--;
		AttributeComponent->HealPlayer(PotionHealAmount);

		BroadcastPotionUpdate();
	}
}

void UGSPotionInventoryComponent::SpawnPotion()
{
	PotionActor = GetWorld()->SpawnActor<AGSPotion>(PotionClass, GetOwner()->GetActorTransform());
	if (PotionActor)
	{
		if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			PotionActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), PotionSocketName);
		}
	}
}

void UGSPotionInventoryComponent::DespawnPotion()
{
	if (IsValid(PotionActor))
	{
		PotionActor->Destroy();
	}
}

void UGSPotionInventoryComponent::SetPointQuantity(uint8 InQuantity)
{
	PotionQuantity = InQuantity;

	BroadcastPotionUpdate();
}

void UGSPotionInventoryComponent::BroadcastPotionUpdate() const
{
	if (OnUpdatePotionAmount.IsBound())
	{
		OnUpdatePotionAmount.Broadcast(PotionQuantity);
	}
}
