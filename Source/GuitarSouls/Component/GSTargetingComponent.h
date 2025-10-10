// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GSEnumDefine.h"
#include "GSTargetingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Trace Range
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TargetingRadius = 500.f;

	// Lock on Camera Rotation speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FaceLockOnRotationSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	//Cache Character & Camera
	UPROPERTY()
	TObjectPtr<class ACharacter> Character;
	UPROPERTY()
	TObjectPtr<class UCameraComponent> Camera;

	// Locked on Target
	UPROPERTY()
	AActor* LockedTargetActor;

	bool bIsLockOn = false;
	
public:	
	UGSTargetingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ToggleLockOn();
	void SwitchingLockedOnActor(ESwitchingDirection InDirection);

	FORCEINLINE bool IsLockedOn() const { return bIsLockOn; }

protected:
	void FindTargets(OUT TArray<AActor*>& OutTargetActors) const;

	AActor* FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDirection = ESwitchingDirection::None) const;

	// Sync Camera Rotation & Character
	void OrientCamera() const;

	void OrientMovement() const;

	void FaceLockOnActor() const;

	void LockOnTarget();

	void StopLockOn();
		
};
