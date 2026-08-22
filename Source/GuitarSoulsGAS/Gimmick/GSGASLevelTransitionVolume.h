// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSGASLevelTransitionVolume.generated.h"

// 소울라이크 안개문 스타일 레벨 전환 트리거.
// 플레이어가 박스 볼륨에 닿으면 현재 상태를 지속 계층에 저장하고 TargetLevel로 OpenLevel한다.
UCLASS()
class GUITARSOULSGAS_API AGSGASLevelTransitionVolume : public AActor
{
	GENERATED_BODY()

public:
	AGSGASLevelTransitionVolume();

protected:
	virtual void BeginPlay() override;

	// 활성화 지연이 끝나면 발동 허용 (스폰 직후 되튀김 방지)
	void EnableTrigger() { bReady = true; }

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 트리거 볼륨 (루트) — Pawn만 Overlap
	UPROPERTY(VisibleAnywhere, Category = "Transition")
	TObjectPtr<class UBoxComponent> TriggerBox;

	// 이동 목표 맵 — 에디터에서 .umap 드롭다운으로 지정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	TSoftObjectPtr<UWorld> TargetLevel;

	// 되튀김 방지: 스폰(BeginPlay) 직후 이 시간 동안 발동을 억제
	UPROPERTY(EditAnywhere, Category = "Transition")
	float ActivationDelay = 0.3f;

private:
	bool bReady = false;
	bool bIsTransitioning = false;
	FTimerHandle ActivationTimerHandle;
};
