// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/GSGASRunTimerSubsystem.h"
#include "GuitarSoulsGAS.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	const FString LeaderboardSlotName = TEXT("Leaderboard");
	constexpr int32 LeaderboardUserIndex = 0;
}

void UGSGASRunTimerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayStatics::DoesSaveGameExist(LeaderboardSlotName, LeaderboardUserIndex))
	{
		LeaderboardSave = Cast<UGSGASLeaderboardSave>(UGameplayStatics::LoadGameFromSlot(LeaderboardSlotName, LeaderboardUserIndex));
	}

	if (!LeaderboardSave)
	{
		LeaderboardSave = Cast<UGSGASLeaderboardSave>(UGameplayStatics::CreateSaveGameObject(UGSGASLeaderboardSave::StaticClass()));
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("Leaderboard loaded. (%d entries)"), LeaderboardSave ? LeaderboardSave->Entries.Num() : 0);
}

void UGSGASRunTimerSubsystem::StartRun()
{
	StartTime = FPlatformTime::Seconds();
	FinalTime = 0.f;
	bRunning = true;
	bFinished = false;
	GSGAS_LOG(LogGSGAS, Log, TEXT("Run started."));
}

void UGSGASRunTimerSubsystem::StopRun()
{
	if (!bRunning)
	{
		return;
	}

	FinalTime = static_cast<float>(FPlatformTime::Seconds() - StartTime);
	bRunning = false;
	bFinished = true;
	GSGAS_LOG(LogGSGAS, Log, TEXT("Run stopped. (%s)"), *FormatTime(FinalTime).ToString());
}

float UGSGASRunTimerSubsystem::GetElapsedSeconds() const
{
	return bRunning ? static_cast<float>(FPlatformTime::Seconds() - StartTime) : FinalTime;
}

int32 UGSGASRunTimerSubsystem::GetRankForTime(float TimeSeconds) const
{
	int32 Rank = 1;
	for (const FGSGASLeaderboardEntry& Entry : GetEntries())
	{
		if (Entry.TimeSeconds <= TimeSeconds)
		{
			++Rank;
		}
	}
	return Rank <= MaxEntries ? Rank : 0;
}

bool UGSGASRunTimerSubsystem::SubmitRecord(const FString& Name)
{
	if (!bFinished || !LeaderboardSave || !IsValidRecordName(Name))
	{
		return false;
	}

	const int32 Rank = GetRankForTime(FinalTime);
	if (Rank == 0)
	{
		return false;
	}

	FGSGASLeaderboardEntry NewEntry;
	NewEntry.Name = Name;
	NewEntry.TimeSeconds = FinalTime;
	LeaderboardSave->Entries.Insert(NewEntry, Rank - 1);
	LeaderboardSave->Entries.SetNum(FMath::Min(LeaderboardSave->Entries.Num(), MaxEntries));

	UGameplayStatics::SaveGameToSlot(LeaderboardSave, LeaderboardSlotName, LeaderboardUserIndex);
	bFinished = false;

	GSGAS_LOG(LogGSGAS, Log, TEXT("Record submitted: #%d %s %s"), Rank, *Name, *FormatTime(FinalTime).ToString());
	return true;
}

const TArray<FGSGASLeaderboardEntry>& UGSGASRunTimerSubsystem::GetEntries() const
{
	static const TArray<FGSGASLeaderboardEntry> Empty;
	return LeaderboardSave ? LeaderboardSave->Entries : Empty;
}

FText UGSGASRunTimerSubsystem::FormatTime(float Seconds)
{
	const int32 TotalCentiseconds = FMath::FloorToInt(FMath::Max(Seconds, 0.f) * 100.f);
	return FText::FromString(FString::Printf(TEXT("%02d:%02d.%02d"),
		TotalCentiseconds / 6000, (TotalCentiseconds / 100) % 60, TotalCentiseconds % 100));
}

bool UGSGASRunTimerSubsystem::IsValidRecordName(const FString& Name)
{
	if (Name.Len() != 5)
	{
		return false;
	}

	for (int32 i = 0; i < 5; ++i)
	{
		const TCHAR C = Name[i];
		const bool bValid = (i < 3) ? (C >= TEXT('A') && C <= TEXT('Z')) : (C >= TEXT('0') && C <= TEXT('9'));
		if (!bValid)
		{
			return false;
		}
	}
	return true;
}
