// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GSGASInteractInterface.h"
#include "GSGASInteractableActor.generated.h"

/**
 * 상호작용 액터 공통 베이스.
 *
 * 인터페이스(UINTERFACE)는 UPROPERTY를 가질 수 없으므로, 모든 구현체가 공유하는
 * 프롬프트 문구 필드를 여기 한 곳에 둔다. 구현체마다 같은 필드를 반복 선언하지 않기 위함.
 *
 * 사용법:
 * - C++ 액터: 이 클래스를 상속하고 Interact_Implementation만 구현
 * - BP 액터: Parent Class를 이 클래스로 지정 → 디테일 패널에서 문구 입력, Interact 이벤트 구현
 *
 * 이 클래스를 거치지 않고 IGSGASInteractInterface를 직접 구현해도 동작한다.
 * 그 경우 문구는 인터페이스 기본값(자리표시자)이 나오므로 GetInteractPromptText를 직접 오버라이드할 것.
 */
UCLASS(Abstract)
class GUITARSOULSGAS_API AGSGASInteractableActor : public AActor, public IGSGASInteractInterface
{
	GENERATED_BODY()

public:
	AGSGASInteractableActor();

	// 기본 동작 없음 — 파생 C++ 클래스나 BP의 Interact 이벤트에서 구현한다.
	virtual void Interact_Implementation(AActor* InteractionActor) override;

	// 아래 InteractPromptText를 그대로 반환.
	// BP에서 Get Interact Prompt Text 이벤트를 오버라이드하면 그쪽이 우선한다(이 필드는 무시됨).
	virtual FText GetInteractPromptText_Implementation() const override;

protected:
	// 상호작용 범위 진입 시 HUD에 뜰 문구. 레벨의 개별 인스턴스마다 다르게 줄 수 있다.
	// 기본값을 그대로 두면 화면에 자리표시자가 보이므로 미설정을 바로 알 수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	FText InteractPromptText = FText::FromString(TEXT("Write InteractPromptText Here"));
};
