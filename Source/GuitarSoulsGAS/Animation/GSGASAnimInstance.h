// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GSGASAnimInstance.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGSGASAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGSGASAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References)
	TObjectPtr<class UAbilitySystemComponent> ASC;

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
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CombatData)
	uint8 bCombatEnabled : 1;

	// 0=Unarmed, 1=Sword, 2=TwoHanded, 3=Fist
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatData)
	int32 CombatPoseIndex;
};
