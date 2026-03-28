// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Roll.generated.h"

UENUM(BlueprintType)
enum class ERollDirection : uint8
{
	None		UMETA(DisplayName = "None"),
	Forward		UMETA(DisplayName = "Forward"),
	Backward	UMETA(DisplayName = "Backward"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right")
};
/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_Roll : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Roll();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> ForwardMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> BackwardMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> LeftMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> RightMontage = nullptr;
	
};
