// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GSGASPlayerState.generated.h"

UCLASS()
class GUITARSOULSGAS_API AGSGASPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGSGASPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE class UGSAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UGSAttributeSet> AttributeSet;

};
