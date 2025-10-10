// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/GSAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/GSCharacterPlayer.h"
#include "Component/GSCombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGSAnimInstance::UGSAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UGSAnimInstance::AnimNotify_ResetMovementInput()
{
	if (AGSCharacterPlayer* LocalCharacter = Cast<AGSCharacterPlayer>(GetOwningActor()))
	{
		LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
	}
}

void UGSAnimInstance::AnimNotify_ResetState()
{
	if (AGSCharacterPlayer* LocalCharacter = Cast<AGSCharacterPlayer>(GetOwningActor()))
	{
		LocalCharacter->GetStateComponent()->ClearState();
	}
}

void UGSAnimInstance::UpdateCombatMode(const ECombatType InCombatType)
{
	CombatType = InCombatType;
}

void UGSAnimInstance::UpdateBlocking(bool InShouldBlocking)
{
	bShouldBlocking = InShouldBlocking;
}

void UGSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
		if (UGSCombatComponent* CombatComponent = Owner->GetComponentByClass<UGSCombatComponent>())
		{
			CombatComponent->OnChangeCombat.AddUObject(this, &UGSAnimInstance::OnChangedCombat);
		}
	}
}

void UGSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Movement == nullptr)
	{
		return;
	}
	
	Velocity = Movement->Velocity;
	GroundSpeed = Velocity.Size2D();
	//bIsIdle = GroundSpeed < MovingThreshould;
	bShouldMove = GroundSpeed > MovingThreshould && Movement->GetCurrentAcceleration() != FVector::ZeroVector;
	bIsFalling = Movement->IsFalling();

	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Owner->GetActorRotation());
}

void UGSAnimInstance::OnChangedCombat(const bool bInCombatEnabled)
{
	bCombatEnabled = bInCombatEnabled;
	
}
