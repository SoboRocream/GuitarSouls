// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/GSCombatInterface.h"
#include "Interface/GSTargetingInterface.h"
#include "GSTestEnemy.generated.h"

UCLASS()
class GUITARSOULS_API AGSTestEnemy : public ACharacter, public IGSTargetingInterface, public IGSCombatInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSCharacterStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> TargetingSphereComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> LockOnWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> HpBarWidgetComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGSRotationComponent> RotationComponent;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class UParticleSystem> ImpactParticle;

// Patrol Section	
protected:
	UPROPERTY(EditAnywhere, Category="AI | Patrol")
	TArray<class ATargetPoint*> PatrolPoints;

	UPROPERTY(EditAnywhere, Category="AI | Patrol")
	int32 PatrolIndex = 0;

// Combat Section	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AGSWeapon> DefaultWeaponClass;

	FTimerHandle ParriedDelayTimerHandle;
	FTimerHandle StunnedDelayTimerHandle;

	UPROPERTY(EditAnywhere)
	uint8 StunnedRate = 0;

	UPROPERTY(EditAnywhere)
	float StunnedDelay = 0.f;
	
public:
	AGSTestEnemy();

	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void SetDeathState();
	void OnAttributeChanged(EGSEnumDefine AttributeType, float InValue);
	void SetupHpBar();

protected:
	void ImpactEffect(const FVector& Location);
	virtual void HitReaction(const AActor* Attacker);

public:
	virtual void OnTargeted(bool bTargeted) override;
	virtual bool CanBeTargeted() override;

	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) override;
	virtual void Parried() override;

	void ToggleHpBarVisibility(bool bVisibility);

	virtual void WatchTarget(AActor* InTargetActor);

public:
	FORCEINLINE ATargetPoint* GetTargetPoint() { return PatrolPoints.Num() >= (PatrolIndex + 1) ? PatrolPoints[PatrolIndex] : nullptr; }
	FORCEINLINE void IncrementPatrolIndex() { PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num() ; }
};
