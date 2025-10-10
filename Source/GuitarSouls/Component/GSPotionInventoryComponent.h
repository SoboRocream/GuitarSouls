// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSPotionInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnUpdatePotionAmount, uint8);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSPotionInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FDelegateOnUpdatePotionAmount OnUpdatePotionAmount;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 PotionQuantity = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PotionHealAmount = 15.f;

	UPROPERTY(EditAnywhere)
	FName PotionSocketName;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGSPotion> PotionClass;
	
	UPROPERTY()
	TObjectPtr<class AGSPotion> PotionActor;
public:	
	UGSPotionInventoryComponent();

public:
	void DrinkPotion();

	void SpawnPotion();

	void DespawnPotion();

	void SetPointQuantity(uint8 InQuantity);

	FORCEINLINE uint8 GetPotionQuantity() const { return PotionQuantity; }

	void BroadcastPotionUpdate() const;
};
