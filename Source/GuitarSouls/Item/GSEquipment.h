// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSEquipment.generated.h"

UCLASS()
class GUITARSOULS_API AGSEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGSEquipment();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void EquipItem();
	virtual void UnEquipItem();
	virtual void AttachToOwner(FName SocketName);

	FORCEINLINE UTexture2D* GetEquipmentIcon() const { return EquipmentIcon; }

public:
	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	TObjectPtr<UStaticMesh> MeshAsset;

	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	TObjectPtr<UMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	TObjectPtr<UTexture2D> EquipmentIcon;


};
