// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "GSGASCharacterBase.generated.h"

UCLASS()
class GUITARSOULSGAS_API AGSGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGSGASCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void SetEquippedWeapon(class AGSGASWeapon* InWeapon);
	FORCEINLINE class AGSGASWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	
	void SetCombatEnabled(bool bEnabled);
	
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

//GAS Section
protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category=GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	// 무기
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AGSGASWeapon> EquippedWeapon;
	
	bool bCombatEnabled = false;
	
	UPROPERTY(EditAnywhere, Category = "GAS|Bridge")
	TSubclassOf<class UGameplayEffect> TakeDamageBridgeEffectClass;
};
