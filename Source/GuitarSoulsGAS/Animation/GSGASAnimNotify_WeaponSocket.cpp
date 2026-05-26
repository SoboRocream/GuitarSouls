// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/GSGASAnimNotify_WeaponSocket.h"
#include "Character/GSGASCharacterBase.h"
#include "Item/GSGASWeapon.h"
#include "GuitarSoulsGAS.h"

void UGSGASAnimNotify_WeaponSocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AGSGASCharacterBase* Character = MeshComp ? Cast<AGSGASCharacterBase>(MeshComp->GetOwner()) : nullptr;
	if (!Character)
	{
		return;
	}

	AGSGASWeapon* Weapon = Character->GetEquippedWeapon();
	if (!Weapon)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AnimNotify_WeaponSocket: No equipped weapon on %s."), *Character->GetName());
		return;
	}

	Weapon->SwitchSocket(bEquip);

	GSGAS_LOG(LogGSGAS, Log, TEXT("AnimNotify_WeaponSocket: %s → %s"),
		*Character->GetName(),
		bEquip ? TEXT("EquipSocket") : TEXT("UnEquipSocket"));
}

FString UGSGASAnimNotify_WeaponSocket::GetNotifyName_Implementation() const
{
	return bEquip ? TEXT("WeaponSocket_Equip") : TEXT("WeaponSocket_Unequip");
}
