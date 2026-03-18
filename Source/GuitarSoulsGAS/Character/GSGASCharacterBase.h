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

	FORCEINLINE class UGSGASWeaponCollisionComponent* GetWeaponCollision() const { return WeaponCollision; }
	FORCEINLINE class UGSGASWeaponData* GetWeaponData() const { return WeaponData; }

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

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<class UGSGASWeaponCollisionComponent> WeaponCollision;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<class UGSGASWeaponData> WeaponData;

	UPROPERTY(EditAnywhere, Category = "GAS|Bridge")
	TSubclassOf<class UGameplayEffect> TakeDamageBridgeEffectClass;
};
