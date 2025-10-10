// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSPlayerHUDWidget.h"

#include "GSPotionWidget.h"
#include "GSStatBarWidget.h"
#include "Component/GSAttributeComponent.h"
#include "GSEnumDefine.h"
#include "GSWeaponWidget.h"
#include "Component/GSCombatComponent.h"
#include "Component/GSPotionInventoryComponent.h"

UGSPlayerHUDWidget::UGSPlayerHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UGSPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UGSAttributeComponent* Attribute = OwningPawn->GetComponentByClass<UGSAttributeComponent>())
		{
			Attribute->OnAttributeChanged.AddUObject(this, &UGSPlayerHUDWidget::OnAttributeChanged);
			Attribute->BroadCastAttributeChanged(EGSEnumDefine::Stamina);
			Attribute->BroadCastAttributeChanged(EGSEnumDefine::Health);
		}

		if (UGSPotionInventoryComponent* PotionInventoryComponent = OwningPawn->GetComponentByClass<UGSPotionInventoryComponent>())
		{
			PotionInventoryComponent->OnUpdatePotionAmount.AddUObject(this, &UGSPlayerHUDWidget::OnPotionQuantityChanged);
			PotionInventoryComponent->BroadcastPotionUpdate();
		}

		if (UGSCombatComponent* CombatComponent = OwningPawn->GetComponentByClass<UGSCombatComponent>())
		{
			CombatComponent->OnChangedWeapon.AddUObject(this, &UGSPlayerHUDWidget::OnWeaponChanged);
			CombatComponent->OnChangedWeapon.Broadcast();
		}
	}

	
}

void UGSPlayerHUDWidget::OnAttributeChanged(EGSEnumDefine AttributeType, float InValue)
{
	switch (AttributeType)
	{
		case EGSEnumDefine::Stamina:
		StaminaBarWidget->SetRatio(InValue);
		break;
		case EGSEnumDefine::Health:
		HpBarWidget->SetRatio(InValue);
		break;
	}
}

void UGSPlayerHUDWidget::OnPotionQuantityChanged(uint8 InAmount)
{
	if (PotionWidget)
	{
		PotionWidget->SetPotionQuantity(InAmount);
	}
}

void UGSPlayerHUDWidget::OnWeaponChanged()
{
	if (const APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (const UGSCombatComponent* CombatComponent = OwningPawn->GetComponentByClass<UGSCombatComponent>())
		{
			UTexture2D* WeaponIconTexture = BlankIcon;
			UTexture2D* ShieldIconTexture = BlankIcon;

			if (const AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon())
			{
				WeaponIconTexture = MainWeapon->GetEquipmentIcon();
			}

			if (const AGSShield* Shield = CombatComponent->GetShield())
			{
				ShieldIconTexture = Shield->GetEquipmentIcon();
			}

			if (::IsValid(WeaponWidget))
			{
				WeaponWidget->SetWeaponImage(WeaponIconTexture);
			}

			if (::IsValid(ShieldWidget))
			{
				ShieldWidget->SetWeaponImage(ShieldIconTexture);
			}
		}
	}
}
