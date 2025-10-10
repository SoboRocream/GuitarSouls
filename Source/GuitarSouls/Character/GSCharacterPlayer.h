// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSCharacterStateComponent.h"
#include "Character/GSCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/GSCombatInterface.h"
#include "GSCharacterPlayer.generated.h"

UCLASS()
class GUITARSOULS_API AGSCharacterPlayer : public AGSCharacterBase, public IGSCombatInterface
{
	GENERATED_BODY()
public:
	AGSCharacterPlayer();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	
	FORCEINLINE UGSCharacterStateComponent* GetStateComponent() const { return StateComponent; }
	bool IsDeath() const;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ImpactEffect(const FVector& Location);
	void ShieldBlockEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker, const EGSDamageType InDamageType);
	void OnDeath();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
// Character Control Section
protected:
	void ChangeCharacterControl();
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class UGSCharacterControlData* CharacterControlData) override;
	
// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintRollingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ToggleCombatAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LockOnTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LeftTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RightTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ParryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> UseItemAction;

protected:
	UPROPERTY(EditAnywhere, Category=MovementSpeed)
	float SprintSpeed = 750.f;
	
	UPROPERTY(EditAnywhere, Category=MovementSpeed)
	float NormalSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category=MovementSpeed)
	float BlockingSpeed = 250.f;
	
	UPROPERTY(VisibleAnywhere, Category=MovementSpeed)
	bool bSprinting = false;

	FVector2D LastMovementInput;

	FORCEINLINE bool IsSprinting() const { return bSprinting; }
	FORCEINLINE bool CanReceiveDamage() const { return !bEnabledIFrames; }

	ECharacterControlType CurrentCharacterControlType;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Character State Section
	bool IsMoving() const;
	bool CanToggleCombat() const;

	// Character Ability Section
	void Sprint();
	void StopSprint();
	void Rolling();
	void Interact();
	void ToggleCombat();
	void AutoToggleCombat();

	// Attack
	void Attack();
	void HeavyAttack();
	void SpecialAttack();

	// Combo Section
protected:
	bool bComboSequenceRunning = false;
	bool bCanComboInput = false;
	int32 ComboCounter = 0;
	bool bSavedComboInput = false;
	FTimerHandle ComboResetTimerHandle;

	FGameplayTag GetAttackPerform() const;

	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	void DoAttack(const FGameplayTag& AttackTypeTag);
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	void ResetCombo();

	bool CanPlayerBlockStance() const;
	bool CanPerformBlocking() const;
	bool CanPerformParry() const;
	bool CanDrinkPotion() const;

	void InterruptWhileDrinking();

	bool ParriedAttackSucceed() const;

	// 무적 프레임
	bool bEnabledIFrames = false;
	
public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

	// LockOn
	void LockOnTarget();
	void LeftTarget();
	void RightTarget();

	void Blocking();
	void BlockingEnd();

	void Parrying();

	void UsePotion();

protected:
	bool bFacingEnemy = false;
	
// Component Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSCharacterStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSTargetingComponent> TargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSPotionInventoryComponent> PotionInventoryComponent;

// UI Widget Section
protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGSUserWidget> PlayerHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	TObjectPtr<class UGSPlayerHUDWidget> PlayerHUDWidget;

// Default Weapon Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AGSFistWeapon> FistWeaponClass;
	
// Anim Section	
protected:
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> RollingMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> DrinkingMontage;

// Effect	
protected:
	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class USoundCue> BlockSound;

	UPROPERTY(EditAnywhere, Category = Effect)
	TObjectPtr<class UParticleSystem> BlockParticle;
	
public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void ToggleIFrames(const bool bEnabled) override;
};
