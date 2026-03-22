// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSGASInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGSGASInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GUITARSOULSGAS_API IGSGASInteractInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* InteractionActor) = 0;
};
