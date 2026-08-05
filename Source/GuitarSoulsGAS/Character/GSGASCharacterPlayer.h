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

	// [전시용] 광폭화 방식 전환 데모 ------------------------------------------------
	// true면 컴포넌트 방식, false면 GAS(BPGE_Berserk) 방식으로 광폭화를 처리한다.
	// 공격 GA가 적중 시 이 값을 보고 분기한다.
	FORCEINLINE bool IsUsingComponentBerserk() const { return bUseComponentBerserk; }
	FORCEINLINE class UGSGASBerserkComponent* GetBerserkComponent() const { return BerserkComponent; }

	// GAS ↔ 컴포넌트 모드 전환 (BP 입력 노드에서 호출).
	// 양쪽 스택을 초기화해 혼선을 막고, 현재 모드를 화면에 표시한다.
	UFUNCTION(BlueprintCallable, Category = "Berserk Demo")
	void ToggleBerserkMode();

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

	// [전시용] 광폭화 데모 Section
protected:
	// 컴포넌트 방식 광폭화 구현체 (GAS 대조군)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Berserk Demo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGSGASBerserkComponent> BerserkComponent;

	// true = 컴포넌트 방식, false = GAS 방식 (BP 입력에서 ToggleBerserkMode로 전환)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Berserk Demo", meta = (AllowPrivateAccess = "true"))
	bool bUseComponentBerserk = false;

	// 모드 전환 시 남은 GAS 광폭화 스택을 즉시 제거하기 위한 참조 (BPGE_Berserk 지정). 선택.
	UPROPERTY(EditAnywhere, Category = "Berserk Demo", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> BerserkGASEffectClass;
};
