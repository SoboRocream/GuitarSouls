// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/GSArmour.h"
#include "Item/GSShield.h"
#include "Item/GSWeapon.h"
#include "GSCombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnChangeCombat, bool);
DECLARE_MULTICAST_DELEGATE(FDelegateOnChangedWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties 
	
	void SetWeapon(AGSWeapon* NewWeapon);
	void SetArmour(AGSArmour* NewArmour);
	void SetShield(AGSShield* NewShield);

	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	FORCEINLINE bool IsBlockingEnabled() const { return bBlockingEnabled; }
	FORCEINLINE void SetBlockingEnabled(const bool bEnabled) { bBlockingEnabled = bEnabled; }
	
	FORCEINLINE AGSWeapon* GetMainWeapon() const {return MainWeapon;}
	FORCEINLINE AGSArmour* GetArmour(const EGSArmourType ArmourType) const
	{
		if (const TObjectPtr<AGSArmour>* FoundArmourPtr = ArmourMap.Find(ArmourType))
		{
			return FoundArmourPtr->Get();
		}
		return nullptr;
	}
	FORCEINLINE AGSShield* GetShield() const {return Shield;}

	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType;}
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackType) { LastAttackType = NewAttackType;}

	void SetCombatEnabled(const bool bEnabled);

private:
	void SpawnPickupItem(const AActor* OwnerActor, const TSubclassOf<AGSEquipment>& NewEquipmentCLass) const;

public:
	FDelegateOnChangeCombat OnChangeCombat;

	FDelegateOnChangedWeapon OnChangedWeapon;
	
protected:
	UPROPERTY()
	TObjectPtr<class AGSWeapon> MainWeapon;

	UPROPERTY()
	TMap<EGSArmourType, TObjectPtr<class AGSArmour>> ArmourMap;

	UPROPERTY()
	TObjectPtr<class AGSShield> Shield;

	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;

	UPROPERTY()
	bool bBlockingEnabled = false;

	UPROPERTY(EditAnywhere)
	FGameplayTag LastAttackType;
	
};
