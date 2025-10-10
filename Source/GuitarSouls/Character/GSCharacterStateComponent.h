// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Tag/GSGameplayTags.h"
#include "GSCharacterStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGSCharacterStateComponent();

	FORCEINLINE void SetState(const FGameplayTag NewState)
	{
		if (CurrentState == GSGameplayTags::Character_State_Death)
		{
			return;
		}
		CurrentState = NewState;
	};
	FORCEINLINE FGameplayTag GetState() const { return CurrentState; };

	FORCEINLINE bool MovementInputEnabled() const { return bMovementInputEnabled; };
	
	bool IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const;
	void ToggleMovementInput(bool bEnabled, float Duration = 0.f);

	void ClearState();

protected:
	UFUNCTION()
	void MovementInputEnableAction();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	FGameplayTag CurrentState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	bool bMovementInputEnabled = true;	
};
