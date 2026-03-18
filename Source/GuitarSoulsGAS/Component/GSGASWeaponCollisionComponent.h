// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSGASWeaponCollisionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGSGASOnHitActor, const FHitResult&);

/**
 * - 소켓 기반 SphereTraceMulti로 매 Tick 판정
 * - TurnOnCollision() 시 AlreadyHitActors 초기화 (한 스윙 중복 히트 방지)
 * - Tick은 비활성 상태로 시작, TurnOnCollision/TurnOffCollision 시에만 활성화
 * - OnHitActor 델리게이트로 히트 결과를 GA에 전달
 *
 * 사용 흐름:
 *   GSGASAnimNotifyState_WeaponCollision → ASC 태그 추가/제거
 *   GA_LightAttack / GA_HeavyAttack → 태그 감지 → TurnOnCollision/TurnOffCollision 호출
 *   GA → OnHitActor 구독 → GE_GSDamage Apply
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULSGAS_API UGSGASWeaponCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGSGASWeaponCollisionComponent();

	void TurnOnCollision();
	void TurnOffCollision();

	void SetWeaponMesh(UPrimitiveComponent* InMesh);
 
	void AddIgnoreActor(AActor* InActor);
	void RemoveIgnoreActor(AActor* InActor);

	FORCEINLINE bool IsCollisionEnabled() const { return bCollisionEnabled;}
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// GA가 구독 — 히트 발생 시마다 브로드캐스트
	FGSGASOnHitActor OnHitActor;
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	FName TraceStartSocketName;
 
	UPROPERTY(EditAnywhere, Category = "Collision")
	FName TraceEndSocketName;
 
	UPROPERTY(EditAnywhere, Category = "Collision")
	float TraceRadius = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

protected:
	bool bCollisionEnabled = false;
 
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> WeaponMesh;
 
	UPROPERTY()
	TArray<TObjectPtr<AActor>> IgnoreActors;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> AlreadyHitActors;
 
	bool CanHitActor(const AActor* InActor) const;
	void PerformCollisionTrace();
};
