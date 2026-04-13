// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_HitReaction.h"

#include "GuitarSoulsGAS.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GSGASCharacterBase.h"
#include "Item/GSGASWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/GSGASGameplayTags.h"

UGA_HitReaction::UGA_HitReaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// C++에서 명시적 트리거 설정
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = GSGASGameplayTags::Character_Action_HitReaction;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_HitReaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_HitReaction: AvatarActor is not AGSGASCharacterBase."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* Montage = nullptr;
	if (AGSGASWeapon* Weapon = Character->GetEquippedWeapon())
	{
		Montage = Weapon->GetMontageForTag(HitReactionTag);
	}
	if (!Montage)
	{
		Montage = DefaultHitReactionMontage;
	}
	if (!Montage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_HitReaction: No montage available."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FName SectionName = NAME_None;
	if (TriggerEventData && IsValid(TriggerEventData->Instigator))
	{
		SectionName = GetHitDirectionSectionName(
			Character->GetActorLocation(),
			Character->GetActorRotation(),
			TriggerEventData->Instigator->GetActorLocation());

		//Debug Log
		const FVector VictimLoc = Character->GetActorLocation();
		const FRotator VictimRot = Character->GetActorRotation();
		const FVector AttackerLoc = TriggerEventData->Instigator->GetActorLocation();

		SectionName = GetHitDirectionSectionName(VictimLoc, VictimRot, AttackerLoc);

		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_HitReaction: Victim=[%s] Rot=[%s] Attacker=[%s] → Section=[%s]"),
			*VictimLoc.ToString(),
			*VictimRot.ToString(),
			*AttackerLoc.ToString(),
			*SectionName.ToString());
		//Debug Log
	}
	else
	{
		SectionName = FName(TEXT("Front"));
		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_HitReaction: No Instigator, defaulting to Front."));
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("HitReactonMontage"), Montage, 1.f, SectionName);

	MontageTask->OnCompleted.AddDynamic(this, &UGA_HitReaction::OnMontageEnd);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_HitReaction::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_HitReaction::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
	
}

void UGA_HitReaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	MontageTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_HitReaction::OnMontageEnd()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			   GetCurrentActivationInfo(), true, false);
}

void UGA_HitReaction::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			   GetCurrentActivationInfo(), true, true);
}

FName UGA_HitReaction::GetHitDirectionSectionName(const FVector& VictimLocation, const FRotator& VictimRotation,
	const FVector& AttackerLocation)
{
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(VictimLocation, AttackerLocation);
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(VictimRotation, LookAtRotation);
	const float DeltaYaw = DeltaRotation.Yaw;

	// DeltaYaw 양수 = 공격자가 왼쪽, 음수 = 공격자가 오른쪽
	if (DeltaYaw >= -45.f && DeltaYaw <= 45.f)
	{
		return FName(TEXT("Front"));
	}
	else if (DeltaYaw > 45.f && DeltaYaw <= 135.f)
	{
		return FName(TEXT("Right"));
	}
	else if (DeltaYaw < -45.f && DeltaYaw >= -135.f)
	{
		return FName(TEXT("Left"));
	}
	else
	{
		return FName(TEXT("Back"));
	}
}
