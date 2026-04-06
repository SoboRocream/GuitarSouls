// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_LockOn.h"

#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Component/GSGASTargetingComponent.h"
#include "AbilitySystemComponent.h"
#include "Tags/GSGASGameplayTags.h"


UGA_LockOn::UGA_LockOn()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_Interact Activate"));
	AGSGASCharacterPlayer* AvatarActor = Cast<AGSGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!AvatarActor)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AvatarActor is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UGSGASTargetingComponent* TargetingComp = AvatarActor->GetComponentByClass<UGSGASTargetingComponent>();
	if (!TargetingComp)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("TargetingComponent not found."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = AvatarActor->GetAbilitySystemComponent();
	
	if (TargetingComp->IsLockedOn())
	{
		// 락온 해제
		TargetingComp->StopLockOn();
		GSGAS_LOG(LogGSGAS, Log, TEXT("LockOn released."));
	}
	else
	{
		// 락온 시도
		TargetingComp->LockOn();

		// LockOnTarget() 내부에서 타겟을 찾지 못하면 bIsLockOn이 false로 유지됨
		if (TargetingComp->IsLockedOn())
		{
			GSGAS_LOG(LogGSGAS, Log, TEXT("LockOn acquired."));
		}
		else
		{
			GSGAS_LOG(LogGSGAS, Log, TEXT("LockOn failed: no target."));
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
