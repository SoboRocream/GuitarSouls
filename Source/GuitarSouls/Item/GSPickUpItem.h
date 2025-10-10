// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/GSCombatComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/GSInteractInterface.h"
#include "GSPickUpItem.generated.h"

UCLASS()
class GUITARSOULS_API AGSPickUpItem : public AActor, public IGSInteractInterface
{
	GENERATED_BODY()
	
public:	
	AGSPickUpItem();

	virtual void Interact(AActor* InteractionActor) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE void SetEquipmentClass(const TSubclassOf<class AGSEquipment>& NewEquipmentClass) { EquipmentClass = NewEquipmentClass; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TSubclassOf<class AGSEquipment> EquipmentClass;
};
