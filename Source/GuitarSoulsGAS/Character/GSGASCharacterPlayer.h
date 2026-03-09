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

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// UI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UGSGASPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<class UGSGASPlayerHUDWidget> PlayerHUDWidget;
};
