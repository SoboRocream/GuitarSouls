// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/GSGASPersistenceSubsystem.h"
#include "GuitarSoulsGAS.h"

void UGSGASPersistenceSubsystem::SavePlayerData(const FGSGASPlayerSaveData& InData)
{
	SavedData = InData;
	bHasSaveData = true;
	GSGAS_LOG(LogGSGAS, Log, TEXT("Player save data stored."));
}

bool UGSGASPersistenceSubsystem::TryGetSaveData(FGSGASPlayerSaveData& OutData) const
{
	if (!bHasSaveData)
	{
		return false;
	}

	OutData = SavedData;
	return true;
}

void UGSGASPersistenceSubsystem::ClearSaveData()
{
	SavedData = FGSGASPlayerSaveData();
	bHasSaveData = false;
	GSGAS_LOG(LogGSGAS, Log, TEXT("Player save data cleared."));
}
