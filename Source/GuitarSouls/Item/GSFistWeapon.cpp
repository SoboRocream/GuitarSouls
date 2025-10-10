// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSFistWeapon.h"

#include "Animation/GSAnimInstance.h"
#include "Character/GSCharacterPlayer.h"
#include "Component/GSCombatComponent.h"
#include "Component/GSWeaponCollisionComponent.h"

AGSFistWeapon::AGSFistWeapon()
{
}

void AGSFistWeapon::EquipItem()
{
	CombatComponent = GetOwner()->GetComponentByClass<UGSCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);
		if (AGSCharacterPlayer* OwnerCharacter = Cast<AGSCharacterPlayer>(GetOwner()))
		{
			MainWeaponCollision->SetMeshWeapon(OwnerCharacter->GetMesh());
			SecondWeaponCollision->SetMeshWeapon(OwnerCharacter->GetMesh());

			CombatComponent->SetCombatEnabled(true);

			if (UGSAnimInstance* Anim = Cast<UGSAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}

			MainWeaponCollision->AddIgnoreActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoreActor(OwnerCharacter);
		}
	}
}
