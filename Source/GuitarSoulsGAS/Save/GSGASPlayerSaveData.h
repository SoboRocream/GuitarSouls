// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSGASPlayerSaveData.generated.h"

// 레벨 전환(OpenLevel) 시 GameInstance 계층에 임시 보관되는 플레이어 상태 스냅샷.
// OpenLevel은 World/PlayerState/Character를 전부 파괴하므로, 여기 담아 새 레벨에서 복원한다.
USTRUCT()
struct FGSGASPlayerSaveData
{
	GENERATED_BODY()

	// 현재값(소모 리소스) — GetNumericAttribute로 캡처
	UPROPERTY()
	float Health = 0.f;

	UPROPERTY()
	float Stamina = 0.f;

	UPROPERTY()
	float PotionCount = 0.f;

	// base값(영구 스탯) — GetNumericAttributeBase로 캡처.
	// 광폭화 버프는 AttackPower의 CurrentValue만 올리므로 base로 저장해 버프 영구화를 막는다.
	UPROPERTY()
	float MaxHealth = 0.f;

	UPROPERTY()
	float MaxStamina = 0.f;

	UPROPERTY()
	float MaxPotionCount = 0.f;

	UPROPERTY()
	float AttackPower = 0.f;

	UPROPERTY()
	float Defense = 0.f;

	// 무기 상태 — 클래스 + 전투(손 소켓)/비전투(등 소켓) 여부
	UPROPERTY()
	TSubclassOf<class AGSGASWeapon> EquippedWeaponClass = nullptr;

	UPROPERTY()
	bool bCombatEnabled = false;
};
