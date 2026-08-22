// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/GSGASLevelTransitionVolume.h"
#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Save/GSGASPlayerSaveData.h"
#include "Save/GSGASPersistenceSubsystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Controller.h"

AGSGASLevelTransitionVolume::AGSGASLevelTransitionVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(100.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGSGASLevelTransitionVolume::OnTriggerBeginOverlap);
}

void AGSGASLevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	// 스폰 직후 플레이어가 이미 볼륨과 겹쳐 있어도 즉시 발동하지 않도록 짧게 지연
	if (ActivationDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(ActivationTimerHandle, this,
			&AGSGASLevelTransitionVolume::EnableTrigger, ActivationDelay, false);
	}
	else
	{
		bReady = true;
	}
}

void AGSGASLevelTransitionVolume::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bReady || bIsTransitioning)
	{
		return;
	}

	// 플레이어만 발동 (적 Pawn 무시)
	AGSGASCharacterPlayer* Player = Cast<AGSGASCharacterPlayer>(OtherActor);
	if (!Player)
	{
		return;
	}

	const AController* Controller = Player->GetController();
	if (!Controller || !Controller->IsPlayerController())
	{
		return;
	}

	if (TargetLevel.IsNull())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Level transition aborted: TargetLevel is not set."));
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Level transition aborted: GameInstance is null."));
		return;
	}

	UGSGASPersistenceSubsystem* Persistence = GameInstance->GetSubsystem<UGSGASPersistenceSubsystem>();
	if (!Persistence)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Level transition aborted: PersistenceSubsystem is null."));
		return;
	}

	// 현재 플레이어 상태 캡처 — 실패 시 전환 중단(상태 유실 방지)
	FGSGASPlayerSaveData SaveData;
	if (!Player->CaptureSaveData(SaveData))
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Level transition aborted: CaptureSaveData failed."));
		return;
	}

	bIsTransitioning = true;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Persistence->SavePlayerData(SaveData);

	GSGAS_LOG(LogGSGAS, Log, TEXT("Level transition -> %s"), *TargetLevel.ToString());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetLevel);
}
