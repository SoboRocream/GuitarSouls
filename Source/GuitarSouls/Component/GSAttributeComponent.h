// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSEnumDefine.h"
#include "Components/ActorComponent.h"
#include "GSAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EGSEnumDefine, float)
DECLARE_MULTICAST_DELEGATE(FOnDeath)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGSAttributeComponent();
	FORCEINLINE float GetBaseStamina() const { return BaseStamina; }
	FORCEINLINE float GetBaseHealth() const { return BaseHealth; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE void IncreaseDefense(const float DefenseAmount)
	{
		DefenseStat += DefenseAmount;
	}
	FORCEINLINE void DecreaseDefense(const float DefenseAmount)
	{
		DefenseStat -= DefenseAmount;
	}
	
	FORCEINLINE float GetStaminaRatio() const { return BaseStamina / MaxStamina; }
	FORCEINLINE float GetHealthRatio() const { return BaseHealth / MaxHealth; }
	
public:
	// 스탯 변경 Delegate
	FDelegateOnAttributeChanged OnAttributeChanged;

	FOnDeath OnDeath;

protected:
	UPROPERTY(EditAnywhere, Category = Stat)
	float BaseStamina = 100.f;
	
	UPROPERTY(EditAnywhere, Category = Stat)
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = Stat)
	float StaminaRegenRate = 0.1f;

	UPROPERTY(EditAnywhere, Category = Stat)
	float BaseHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = Stat)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = Stat)
	float DefenseStat = 0.f;
	
	// Stamina Regenerate Timer Handle
	FTimerHandle StaminaRegenTimerHandle;

public:
	bool CheckHasEnoughStamina(float StaminaCost) const;
	void DecreaseStamina(float StaminaCost);
	void ToggleStaminaRegen(bool bEnabled, float RegenDelay = 0.f);

	void BroadCastAttributeChanged(EGSEnumDefine InAttribute) const;

	void TakeDamageAmount(float DamageAmount);

	void HealPlayer(float HealAmount);

protected:
	void RegenerateStamina();
};
