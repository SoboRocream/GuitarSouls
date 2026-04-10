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

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();
	
	void ApplyRollRotation(const FVector2D& MovementInput, ACharacter* Character) const;
	void ApplyStaminaCost();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> ForwardMontage = nullptr;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	TSubclassOf<UGameplayEffect> StaminaCostEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	float StaminaCost = 15.f;
};
