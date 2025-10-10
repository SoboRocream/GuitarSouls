// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSPotion.generated.h"

UCLASS()
class GUITARSOULS_API AGSPotion : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> Mesh;
	
public:	
	AGSPotion();
	
};
