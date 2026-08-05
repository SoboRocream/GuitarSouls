// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSGASBerserkComponent.generated.h"

/**
 * [전시용] 광폭화 버프의 "컴포넌트 방식" 구현 — GAS(BPGE_Berserk) 대조군.
 *
 * GAS에서는 GameplayEffect 데이터 필드(Duration/Stacking/Modifier)만으로 자동 처리되는
 * 스택 누적·만료 타이머·갱신·초기화를 여기서 전부 손으로 재현한다.
 * 데미지 반영도 자동 캡처가 없으므로, 공격 코드가 GetDamageMultiplier()를 직접 조회해
 * SetByCaller 데미지에 곱하는 방식으로 결합된다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUITARSOULSGAS_API UGSGASBerserkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSGASBerserkComponent();

	// 공격 적중 시 호출: 스택 증가(상한 적용) + 만료 타이머 리셋(GAS의 Refresh 재현)
	void OnHitLanded();

	// 현재 데미지 배율 = 1.0 + 스택 × PerStackMultiplier
	float GetDamageMultiplier() const;

	FORCEINLINE int32 GetCurrentStacks() const { return CurrentStacks; }

	// 스택 즉시 초기화 (모드 전환 등)
	void ResetStacks();

protected:
	// GAS의 Stack Limit 대응
	UPROPERTY(EditAnywhere, Category = "Berserk")
	int32 MaxStacks = 5;

	// GAS의 Modifier(AttackPower Add) 스택당 증가량 대응
	UPROPERTY(EditAnywhere, Category = "Berserk")
	float PerStackMultiplier = 0.1f;

	// GAS의 Duration 대응 (미적중 시 만료까지의 시간, 초)
	UPROPERTY(EditAnywhere, Category = "Berserk")
	float Duration = 5.f;

private:
	int32 CurrentStacks = 0;

	// 만료 타이머 — GAS가 데이터로 처리하던 것을 수동 관리
	FTimerHandle ExpireTimerHandle;

	// 만료 콜백: GAS의 Clear Entire Stack on Expiration 재현
	void OnExpired();
};
