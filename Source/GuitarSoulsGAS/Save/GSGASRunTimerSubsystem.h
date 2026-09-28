// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/GSGASLeaderboardSave.h"
#include "GSGASRunTimerSubsystem.generated.h"

// 타임어택 런 타이머 + 순위표.
// GameInstance와 같은 생명주기 → OpenLevel·사망 재시작(RestartLevel)을 넘어 시계가 계속 돈다.
// PersistenceSubsystem과 분리한 이유: 체크포인트는 런 종료 시 지우지만 순위표는 계속 남아야 한다.
//
// 흐름: 타이틀 Start → StartRun / 보스 사망 → StopRun / 승리 위젯 → SubmitRecord
// StartRun 없이 전투 맵을 PIE로 직접 열면 런이 비활성이라 기록되지 않는다.
UCLASS()
class GUITARSOULSGAS_API UGSGASRunTimerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static constexpr int32 MaxEntries = 10;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "GSGAS|RunTimer")
	void StartRun();

	// 진행 중일 때만 유효. 경과 시간을 확정한다.
	UFUNCTION(BlueprintCallable, Category = "GSGAS|RunTimer")
	void StopRun();

	UFUNCTION(BlueprintPure, Category = "GSGAS|RunTimer")
	bool IsRunning() const { return bRunning; }

	// 정지 후 아직 제출되지 않은 기록이 있는가
	UFUNCTION(BlueprintPure, Category = "GSGAS|RunTimer")
	bool IsRunFinished() const { return bFinished; }

	UFUNCTION(BlueprintPure, Category = "GSGAS|RunTimer")
	float GetElapsedSeconds() const;

	// 해당 기록이 들어갈 순위(1~MaxEntries). 순위권 밖이면 0. 동률은 기존 기록이 앞선다.
	int32 GetRankForTime(float TimeSeconds) const;

	// 확정된 기록을 순위표에 넣고 디스크에 저장. 1회만 성공한다.
	bool SubmitRecord(const FString& Name);

	const TArray<FGSGASLeaderboardEntry>& GetEntries() const;

	// mm:ss.cc
	UFUNCTION(BlueprintPure, Category = "GSGAS|RunTimer")
	static FText FormatTime(float Seconds);

	// 대문자 3글자 + 숫자 2자리
	static bool IsValidRecordName(const FString& Name);

private:
	UPROPERTY()
	TObjectPtr<UGSGASLeaderboardSave> LeaderboardSave;

	double StartTime = 0.0;
	float FinalTime = 0.f;
	bool bRunning = false;
	bool bFinished = false;
};
