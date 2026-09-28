// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GSGASLeaderboardSave.generated.h"

USTRUCT(BlueprintType)
struct FGSGASLeaderboardEntry
{
	GENERATED_BODY()

	// 이니셜 3글자 + 숫자 2자리 (예: ABC07)
	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	float TimeSeconds = 0.f;
};

// 타임어택 순위표. 디스크(Saved/SaveGames/Leaderboard.sav)에 저장되어 앱 재시작 후에도 유지된다.
// 초기화하려면 해당 .sav 파일을 지운다.
UCLASS()
class GUITARSOULSGAS_API UGSGASLeaderboardSave : public USaveGame
{
	GENERATED_BODY()

public:
	// 기록 오름차순 정렬 상태로 유지
	UPROPERTY()
	TArray<FGSGASLeaderboardEntry> Entries;
};
