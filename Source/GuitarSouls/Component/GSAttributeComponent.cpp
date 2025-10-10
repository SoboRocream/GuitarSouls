// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSAttributeComponent.h"

#include "Character/GSCharacterStateComponent.h"
#include "Tag/GSGameplayTags.h"

// Sets default values for this component's properties
UGSAttributeComponent::UGSAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool UGSAttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return BaseStamina >= StaminaCost;
}

void UGSAttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.f, MaxStamina);

	BroadCastAttributeChanged(EGSEnumDefine::Stamina);
}

void UGSAttributeComponent::ToggleStaminaRegen(bool bEnabled, float RegenDelay)
{
	if (bEnabled)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &UGSAttributeComponent::RegenerateStamina, 0.1f, true, RegenDelay);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void UGSAttributeComponent::BroadCastAttributeChanged(EGSEnumDefine InAttribute) const
{
	if (OnAttributeChanged.IsBound())
	{
		float Ratio = 0.0f;
		switch (InAttribute)
		{
		case EGSEnumDefine::Stamina:
			Ratio = GetStaminaRatio();
			break;
		case EGSEnumDefine::Health:
			Ratio = GetHealthRatio();
			break;
		}
		OnAttributeChanged.Broadcast(InAttribute, Ratio);
	}
}

void UGSAttributeComponent::TakeDamageAmount(float DamageAmount)
{
	const float MaxDamage = DamageAmount * (DamageAmount / (DamageAmount + DefenseStat));
	const float TotalDamage = FMath::Clamp(DamageAmount, 0, MaxDamage);

	GEngine->AddOnScreenDebugMessage(6, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount:%f, TotalDamage:%f"), DamageAmount, TotalDamage));
	
	BaseHealth = FMath::Clamp(BaseHealth - TotalDamage, 0.f, MaxHealth);

	BroadCastAttributeChanged(EGSEnumDefine::Health);
	
	if (BaseHealth <= 0.f)
	{
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}

		if (UGSCharacterStateComponent* StateComponent = GetOwner()->FindComponentByClass<UGSCharacterStateComponent>())
		{
			StateComponent->SetState(GSGameplayTags::Character_State_Death);
		}
	}
}

void UGSAttributeComponent::HealPlayer(float HealAmount)
{
	BaseHealth = FMath::Clamp(BaseHealth + HealAmount, 0.f, MaxHealth);
	BroadCastAttributeChanged(EGSEnumDefine::Health);
}

void UGSAttributeComponent::RegenerateStamina()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.f, MaxStamina);
	BroadCastAttributeChanged(EGSEnumDefine::Stamina);
	if (BaseStamina >= MaxStamina)
	{
		ToggleStaminaRegen(false);
	}
}

