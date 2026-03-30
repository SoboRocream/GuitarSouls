// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GSGASTargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULSGAS_API UGSGASTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LockOn")
	float TargetingRadius = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LockOn")
	float FaceLockOnRotationSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LockOn")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForOneFrame;

protected:
	UPROPERTY()
	TObjectPtr<class ACharacter> Character;

	UPROPERTY()
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<AActor> LockedTargetActor;

	bool bIsLockOn = false;

public:
	UGSGASTargetingComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void LockOn();
	void StopLockOn();

	FORCEINLINE bool IsLockedOn() const { return bIsLockOn; }
	FORCEINLINE AActor* GetLockedTarget() const { return LockedTargetActor; }
	
protected:
	virtual void BeginPlay() override;

	void FindTargets(TArray<AActor*>& OutTargetActors) const;
	AActor* FindClosestTarget(TArray<AActor*>& InTargets) const;

	bool CanBeTargeted(AActor* InActor) const;
	
	void OrientCamera() const;
	void OrientMovement() const;
	void FaceLockOnActor() const;
		
};
