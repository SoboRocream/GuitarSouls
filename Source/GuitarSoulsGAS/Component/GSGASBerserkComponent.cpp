// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSGASBerserkComponent.h"

#include "TimerManager.h"
#include "Engine/World.h"

UGSGASBerserkComponent::UGSGASBerserkComponent()
{
	// 스택은 이벤트/타이머로만 변하므로 Tick 불필요
	PrimaryComponentTick.bCanEverTick = false;
}

void UGSGASBerserkComponent::OnHitLanded()
{
	CurrentStacks = FMath::Min(CurrentStacks + 1, MaxStacks);

	// 만료 타이머 리셋 (GAS의 Refresh on Successful Application 수동 재현)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ExpireTimerHandle);
		World->GetTimerManager().SetTimer(
			ExpireTimerHandle, this, &UGSGASBerserkComponent::OnExpired, Duration, false);
	}
}

float UGSGASBerserkComponent::GetDamageMultiplier() const
{
	return 1.f + CurrentStacks * PerStackMultiplier;
}

void UGSGASBerserkComponent::ResetStacks()
{
	CurrentStacks = 0;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ExpireTimerHandle);
	}
}

void UGSGASBerserkComponent::OnExpired()
{
	// GAS의 Clear Entire Stack on Expiration 재현
	CurrentStacks = 0;
}
