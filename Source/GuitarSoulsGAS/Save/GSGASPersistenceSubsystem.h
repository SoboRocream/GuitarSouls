// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/GSGASPlayerSaveData.h"
#include "GSGASPersistenceSubsystem.generated.h"

// GameInstance와 같은 생명주기 → OpenLevel(하드 트래블)을 넘어 생존.
// 레벨 전환 직전 플레이어 상태를 저장하고, 새 레벨의 플레이어가 복원 후 Clear한다.
// NOTE: 단일 로컬 플레이어 전용. 멀티플레이어 확장 시 PlayerId 키 맵으로 바꿔야 함.
UCLASS()
class GUITARSOULSGAS_API UGSGASPersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void SavePlayerData(const FGSGASPlayerSaveData& InData);

	// 조회만 수행(삭제하지 않음). 복원 성공 후 별도로 ClearSaveData를 호출해야 데이터 유실이 없다.
	bool TryGetSaveData(FGSGASPlayerSaveData& OutData) const;

	void ClearSaveData();

	bool HasSaveData() const { return bHasSaveData; }

private:
	UPROPERTY()
	FGSGASPlayerSaveData SavedData;

	UPROPERTY()
	bool bHasSaveData = false;
};
