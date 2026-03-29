// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Interact.h"

#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Data/GSGASCollision.h"
#include "Interface/GSGASInteractInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Interact::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		AGSGASCharacterPlayer* Character = Cast<AGSGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			InteractRadius = Character->GetInteractRadius();
		}
	}
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_Interact Activate"));
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AvatarActor is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CCHANNEL_GSGAS_INTERACTION));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(AvatarActor);

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		AvatarActor,
		AvatarActor->GetActorLocation(),
		AvatarActor->GetActorLocation(),
		InteractRadius,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true);

	for (const FHitResult& Hit : OutHits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (IGSGASInteractInterface* Interactable = Cast<IGSGASInteractInterface>(HitActor))
			{
				GSGAS_LOG(LogGSGAS, Log, TEXT("Interacting with %s."), *HitActor->GetName());
				Interactable->Interact(AvatarActor);
				break;
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
