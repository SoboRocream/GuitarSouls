// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSCombatComponent.h"

#include "Character/GSCharacterPlayer.h"
#include "Item/GSPickUpItem.h"

void UGSCombatComponent::SetWeapon(AGSWeapon* NewWeapon)
{
	if (IsValid(MainWeapon))
	{
		if (const AActor* OwnerActor = GetOwner())
		{
			SpawnPickupItem(OwnerActor, MainWeapon->GetClass());
			
			MainWeapon->Destroy();
			
		}
	}
	MainWeapon = NewWeapon;

	if (OnChangedWeapon.IsBound())
	{
		OnChangedWeapon.Broadcast();
	}
}

void UGSCombatComponent::SetArmour(AGSArmour* NewArmour)
{
	
	const EGSArmourType ArmourType = NewArmour->GetArmourType();

	if (AGSArmour* EquippedArmour = GetArmour(ArmourType))
	{
		
		if (IsValid(EquippedArmour))
		{
			
			if (const AActor* OwnerActor = GetOwner())
			{
				SpawnPickupItem(OwnerActor, EquippedArmour->GetClass());
			}
			
			EquippedArmour->UnEquipItem();
			
			EquippedArmour->Destroy();
		}
		
		ArmourMap[ArmourType] = NewArmour;
	}
	else
	{
		ArmourMap.Add(ArmourType, NewArmour);
	}
	
}

void UGSCombatComponent::SetShield(AGSShield* NewShield)
{
	if (IsValid(Shield))
	{
		if (const AActor* OwnerActor = GetOwner())
		{
			SpawnPickupItem(OwnerActor, Shield->GetClass());
			Shield->Destroy();
		}
	}

	Shield = NewShield;

	if (OnChangedWeapon.IsBound())
	{
		OnChangedWeapon.Broadcast();
	}
}

void UGSCombatComponent::SetCombatEnabled(const bool bEnabled)
{
	bCombatEnabled = bEnabled;
	if (OnChangeCombat.IsBound())
	{
		OnChangeCombat.Broadcast(bCombatEnabled);
	}
		
}


void UGSCombatComponent::SpawnPickupItem(const AActor* OwnerActor,
                                         const TSubclassOf<AGSEquipment>& NewEquipmentCLass) const
{
	AGSPickUpItem* PickUpItem = GetWorld()->SpawnActorDeferred<AGSPickUpItem>(AGSPickUpItem::StaticClass(), OwnerActor->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	PickUpItem->SetEquipmentClass(NewEquipmentCLass);
	PickUpItem->FinishSpawning(GetOwner()->GetActorTransform());
}
