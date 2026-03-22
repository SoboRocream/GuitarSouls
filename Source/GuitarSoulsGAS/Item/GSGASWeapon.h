// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interface/GSGASInteractInterface.h"
#include "GSGASWeapon.generated.h"

UCLASS()
class GUITARSOULSGAS_API AGSGASWeapon : public AActor, public IGSGASInteractInterface
{
	GENERATED_BODY()
	
public:	
	AGSGASWeapon();

	virtual void Interact(AActor* InteractionActor) override;

	void EquipItem();
	void Drop();

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag) const;
	float GetStaminaCost(const FGameplayTag& Tag) const;
	float GetFinalDamage(const FGameplayTag& Tag) const;

	FORCEINLINE class UGSGASWeaponCollisionComponent* GetWeaponCollision(int32 Index = 0) const
	{
		return WeaponCollisions.IsValidIndex(Index) ? WeaponCollisions[Index] : nullptr;
	}
	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnEquipSocketName() const { return UnEquipSocketName; }

	
protected:
	void AttachToOwnerCharacter(FName SocketName);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<class UStaticMeshComponent> Mesh;
 
	// Main=0, Second=1 — 생성자에서 Main만 추가, Second는 필요 시 서브클래스에서 추가
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TArray<TObjectPtr<class UGSGASWeaponCollisionComponent>> WeaponCollisions;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Socket")
	FName EquipSocketName;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Socket")
	FName UnEquipSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Data")
	TObjectPtr<class UGSGASWeaponData> WeaponData;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<class USphereComponent> InteractionSphere;

};
