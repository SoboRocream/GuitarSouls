// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_BossHitSound.h"
#include "GuitarSoulsGAS.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/GSGASGameplayTags.h"

UGA_BossHitSound::UGA_BossHitSound()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = GSGASGameplayTags::Character_Action_HitReaction;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_BossHitSound::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HitSound && ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		UGameplayStatics::PlaySoundAtLocation(
			ActorInfo->AvatarActor.Get(),
			HitSound,
			ActorInfo->AvatarActor->GetActorLocation());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_BossHitSound::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
