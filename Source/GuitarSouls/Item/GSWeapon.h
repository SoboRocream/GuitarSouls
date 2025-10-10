// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/GSEquipment.h"
#include "GSEnumDefine.h"
#include "GSWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API AGSWeapon : public AGSEquipment
{
	GENERATED_BODY()
	
public:
	AGSWeapon();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnEquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	ECombatType CombatType = ECombatType::SwordShield;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	TObjectPtr<class UGSMontageActionData> MontageActionData;

// Component Section	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSWeaponCollisionComponent> MainWeaponCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSWeaponCollisionComponent>  SecondWeaponCollision;

	UPROPERTY()
	TObjectPtr<class UGSCombatComponent> CombatComponent;

// Data Section	
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

public:
	virtual void EquipItem() override;
	// virtual void UnEquipItem() override;

	void Drop();

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	UAnimMontage* GetRandomMontageForTag(const FGameplayTag& Tag) const;
	UAnimMontage* GetHitReactMontage(const AActor* Attacker) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;
	
	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnEquipSocketName() const { return UnEquipSocketName; }
	FORCEINLINE UGSWeaponCollisionComponent* GetCollision() const {return MainWeaponCollision;}
	FORCEINLINE ECombatType GetCombatType() const {return CombatType;}

public:
	virtual void ActivateCollision(EWeaponCollisionType InCollisionType);
	virtual void DeactivateCollision(EWeaponCollisionType InCollisionType);

public:
	void OnHitActor(const FHitResult& Hit);
};
