// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GSEnumDefine.h"
#include "GSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGSAnimInstance();

	UFUNCTION()
	void AnimNotify_ResetMovementInput();

	UFUNCTION()
	void AnimNotify_ResetState();

	void UpdateCombatMode(const ECombatType InCombatType);
	void UpdateBlocking(bool InShouldBlocking);

protected:
	// 최초 생성 시 호출
	virtual void NativeInitializeAnimation() override;
	// 매 프레임 마다 호출
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void OnChangedCombat(const bool bInCombatEnabled);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References)
	TObjectPtr<class UCharacterMovementComponent> Movement;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bShouldMove : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Direction;
	
	// Threshould Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float  MovingThreshould;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshould;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatData)
	bool bCombatEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatData)
	bool bShouldBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatData)
	ECombatType CombatType = ECombatType::None;
};
