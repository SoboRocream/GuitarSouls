// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/GSGASPlayerSaveData.h"
#include "GSGASPersistenceSubsystem.generated.h"

// GameInstance와 같은 생명주기 → OpenLevel(하드 트래블)을 넘어 생존.
// 레벨 전환 직전 플레이어 상태를 저장하고, 새 레벨의 플레이어가 복원한다.
//
// 저장 데이터는 "소비"가 아니라 "체크포인트"다 — 복원해도 지우지 않는다.
// 그래야 사망 후 같은 맵을 다시 로드해도 동일 스냅샷으로 부활할 수 있다.
// (지우면 무기가 소실되어 공격 불가 상태가 된다. GA_LightAttack은 무기 없으면 활성화되지 않는다.)
// Clear는 런이 끝났을 때(승리 후 시작 맵 복귀 등)만 호출한다.
//
// NOTE: 단일 로컬 플레이어 전용. 멀티플레이어 확장 시 PlayerId 키 맵으로 바꿔야 함.
UCLASS()
class GUITARSOULSGAS_API UGSGASPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void SavePlayerData(const FGSGASPlayerSaveData& InData);

	// 조회만 수행(삭제하지 않음).
	bool TryGetSaveData(FGSGASPlayerSaveData& OutData) const;

	// 런 종료 시에만 호출할 것. 사망 재시작 경로에서 호출하면 체크포인트가 사라진다.
	UFUNCTION(BlueprintCallable, Category = "GSGAS|Persistence")
	void ClearSaveData();

	// 사망 → 레벨 재시작 예정 표식. 다음 복원에서 리소스를 가득 채워 부활시킨다.
	UFUNCTION(BlueprintCallable, Category = "GSGAS|Persistence")
	void MarkDeathRestart();

	// 표식을 읽고 즉시 내린다(1회성).
	bool ConsumeDeathRestart();

	bool HasSaveData() const { return bHasSaveData; }

private:
	UPROPERTY()
	FGSGASPlayerSaveData SavedData;

	UPROPERTY()
	bool bHasSaveData = false;

	UPROPERTY()
	bool bPendingDeathRestart = false;
};
