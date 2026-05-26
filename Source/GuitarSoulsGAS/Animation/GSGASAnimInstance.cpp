// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/GSGASAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/GSGASGameplayTags.h"

UGSGASAnimInstance::UGSGASAnimInstance()
{
	MovingThreshold = 3.0f;
}

void UGSGASAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UGSGASAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!Movement)
	{
		return;
	}

	// 플레이어는 PossessedBy 이후에 ASC가 초기화되므로 null이면 재시도
	if (!ASC)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
		{
			ASC = ASI->GetAbilitySystemComponent();
		}
	}

	Velocity = Movement->Velocity;
	GroundSpeed = Velocity.Size2D();
	bShouldMove = GroundSpeed > MovingThreshold && Movement->GetCurrentAcceleration() != FVector::ZeroVector;
	bIsFalling = Movement->IsFalling();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Owner->GetActorRotation());

	if (ASC)
	{
		bCombatEnabled = ASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_CombatEnabled);

		if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_CombatType_Sword))
			CombatPoseIndex = 1;
		else if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_CombatType_TwoHanded))
			CombatPoseIndex = 2;
		else if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_CombatType_Fist))
			CombatPoseIndex = 3;
		else
			CombatPoseIndex = 0;
	}
}
