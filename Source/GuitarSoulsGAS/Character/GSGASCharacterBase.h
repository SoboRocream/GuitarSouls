// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"

#include "GSGASCharacterBase.generated.h"

UCLASS()
class GUITARSOULSGAS_API AGSGASCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AGSGASCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// IGameplayTagAssetInterface — BTDecorator_GameplayTagCondition 등이 ASC 태그를 직접 조회할 수 있도록
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void SetEquippedWeapon(class AGSGASWeapon* InWeapon);
	FORCEINLINE class AGSGASWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	
	void SetCombatEnabled(bool bEnabled);
	
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// 콤보 전환 시 바라볼 방향 반환. 기본값 = 현재 방향 유지
	// Player: 락온 타겟 > 이동 입력 > 현재 방향
	virtual FRotator GetComboFacingRotation() const { return GetActorRotation(); }

protected:
	virtual void BeginPlay() override;

	// Enemy/Boss용: 무기 스폰 → EquipSocket 장착 → 전투 모드 즉시 진입 (몽타주 없음)
	// PossessedBy에서 ASC->InitAbilityActorInfo 완료 후 호출해야 함
	void SpawnAndEquipWeaponInCombat(TSubclassOf<class AGSGASWeapon> WeaponClass);

//GAS Section
protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category=GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	// 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AGSGASWeapon> EquippedWeapon;

	bool bCombatEnabled = false;

	// PossessedBy 중복 호출 방어 — 어빌리티 부여/델리게이트 바인딩/무기 스폰이 한 번만 수행되도록 보장
	bool bGASInitialized = false;
	
	UPROPERTY(EditAnywhere, Category = "GAS|Bridge")
	TSubclassOf<class UGameplayEffect> TakeDamageBridgeEffectClass;
};
