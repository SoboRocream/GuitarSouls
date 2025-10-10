// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_GSEquipWeapon.h"

#include "Component/GSCombatComponent.h"
#include "Tag/GSGameplayTags.h"

UAnimNotify_GSEquipWeapon::UAnimNotify_GSEquipWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAnimNotify_GSEquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (const AActor* Owner = MeshComp->GetOwner())
	{
		if (UGSCombatComponent* CombatComponent = Owner->GetComponentByClass<UGSCombatComponent>())
		{
			if (AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon())
			{
				if (EquipmentType == EGSEquipmentType::Weapon)
				{
					bool bCombatEnabled = CombatComponent->IsCombatEnabled();
					FName WeaponSocketName;
					if (MontageActionTag == GSGameplayTags::Character_Action_Equip)
					{
						bCombatEnabled = true;
						WeaponSocketName = MainWeapon->GetEquipSocketName();
					}
					else if (MontageActionTag == GSGameplayTags::Character_Action_Unequip)
					{
						bCombatEnabled = false;
						WeaponSocketName = MainWeapon->GetUnEquipSocketName();
					}

					CombatComponent->SetCombatEnabled(bCombatEnabled);
					MainWeapon->AttachToOwner(WeaponSocketName);
				}

				if (EquipmentType == EGSEquipmentType::Shield)
				{
					if (MainWeapon->GetCombatType() == ECombatType::SwordShield)
					{
						if (AGSShield* Shield = CombatComponent->GetShield())
						{
							FName ShieldSocketName;
							if (MontageActionTag == GSGameplayTags::Character_Action_Equip)
							{
								ShieldSocketName = Shield->GetEquipSocketName();
							}
							else if (MontageActionTag == GSGameplayTags::Character_Action_Unequip)
							{
								ShieldSocketName = Shield->GetUnequipSocketName();
							}

							Shield->AttachToOwner(ShieldSocketName);
						}
					}
				}
			
			}
		}
	}
}
