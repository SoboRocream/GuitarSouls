// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GSGASCharacterBase.h"
#include "InputActionValue.h"
#include "GSGASCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API AGSGASCharacterPlayer : public AGSGASCharacterBase
{
	GENERATED_BODY()
public:
	AGSGASCharacterPlayer();

	virtual void PossessedBy(AController* NewController) override;

	FORCEINLINE float GetInteractRadius() const { return InteractRadius; }
	FORCEINLINE FVector2D GetLastMovementInput() const { return LastMovementInput; }

	virtual FRotator GetComboFacingRotation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// GAS Input Section
protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;
	
	// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> HeavyAttackAction;

	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<class UInputAction> RollAction;

	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<class UInputAction> LockOnAction;
	
	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<class UInputAction> ToggleCombatAction;
	
	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<class UInputAction> UseItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> QuitAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Interact)
	TObjectPtr<class USphereComponent> InteractDetectionSphere;

	UPROPERTY(EditDefaultsOnly, Category=Interact)
	float InteractRadius = 100.f;

	FVector2D LastMovementInput = FVector2D::ZeroVector;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnQuit();
	
	UFUNCTION()
	void OnInteractSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Character Dead Section
protected:
	UFUNCTION()
	void OnOutOfHealth();
	void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeathBP();
	
	// GAS Init Section
protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayEffect>> InitEffects;
	
	// UI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGSGASPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<class UGSGASPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGSGASGameOverWidget> GameOverWidgetClass;

	// 사망 후 GameOver 위젯 표시까지의 지연 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	float GameOverWidgetDelay = 2.f;

private:
	void ShowGameOverWidget();
	FTimerHandle GameOverTimerHandle;
};
