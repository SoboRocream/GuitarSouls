// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSShield.h"

#include "Component/GSCombatComponent.h"

void AGSShield::EquipItem()
{
	Super::EquipItem();

	if (const AActor* OwnerActor = GetOwner())
	{
		if (UGSCombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UGSCombatComponent>())
		{
			CombatComponent->SetShield(this);

			FName AttachSocketName = UnequipSocketName;
			
			const AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon();
			if (IsValid(MainWeapon))
			{
				const ECombatType CombatType = MainWeapon->GetCombatType();
				if (CombatType == ECombatType::SwordShield)
				{
					if (CombatComponent->IsCombatEnabled())
					{
						AttachSocketName = EquipSocketName;
					}
				}
			}

			AttachToOwner(AttachSocketName);
		}
		
	}
}
