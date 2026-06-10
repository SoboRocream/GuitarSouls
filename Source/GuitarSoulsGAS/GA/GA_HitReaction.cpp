// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_HitReaction.h"

#include "GuitarSoulsGAS.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GSGASCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/GSGASWeapon.h"
#include "Kismet/GameplayStatics.h"
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

	// 피격 사운드 — 몽타주 유무와 무관하게 항상 재생 (보스처럼 몽타주 없는 경우 포함)
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(), HitSound, Character->GetActorLocation());
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
		// 몽타주 없음 — 사운드만 재생하고 종료 (보스 등 몽타주 불필요한 캐릭터)
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FName SectionName = NAME_None;
	if (TriggerEventData && IsValid(TriggerEventData->Instigator))
	{
		const FVector VictimLoc = Character->GetActorLocation();
		const FRotator VictimRot = Character->GetActorRotation();
		const FVector AttackerLoc = TriggerEventData->Instigator->GetActorLocation();

		SectionName = GetHitDirectionSectionName(VictimLoc, VictimRot, AttackerLoc);

		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_HitReaction: Victim=[%s] Rot=[%s] Attacker=[%s] → Section=[%s]"),
			*VictimLoc.ToString(),
			*VictimRot.ToString(),
			*AttackerLoc.ToString(),
			*SectionName.ToString());
	}
	else
	{
		SectionName = FName(TEXT("Front"));
		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_HitReaction: No Instigator, defaulting to Front."));
	}

	// 회전 차단 (빙글빙글 방지) + 입력 차단
	// MaxWalkSpeed = 0 은 root motion의 Velocity 적용을 방해하므로 사용 금지.
	// 플레이어: InputComponent->bBlockInput으로 입력만 차단 (CMC는 건드리지 않아 root motion 정상 동작)
	// 적/보스: InputComponent 없음 → 회전 차단만 적용 (BT 이동은 EndAbility 후 자연 재개)
	if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
	{
		bSavedOrientRotationToMovement = CMC->bOrientRotationToMovement;
		CMC->bOrientRotationToMovement = false;
	}
	if (UInputComponent* IC = Character->InputComponent)
	{
		IC->bBlockInput = true;
	}

	// 넉백 — EventMagnitude > 0 인 경우(보스 공격 등)만 적용
	if (TriggerEventData && TriggerEventData->EventMagnitude > 0.f
		&& IsValid(TriggerEventData->Instigator))
	{
		const FVector ToVictim = (Character->GetActorLocation()
			- TriggerEventData->Instigator->GetActorLocation()).GetSafeNormal2D();

		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		CMC->SetMovementMode(MOVE_Falling);
		CMC->Velocity = ToVictim * TriggerEventData->EventMagnitude
			+ FVector(0.f, 0.f, 300.f);
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
	// 회전/입력 복원
	if (ActorInfo)
	{
		if (AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
			{
				CMC->bOrientRotationToMovement = bSavedOrientRotationToMovement;
			}
			if (UInputComponent* IC = Character->InputComponent)
			{
				IC->bBlockInput = false;
			}
		}
	}

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

void UGA_HitReaction::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
