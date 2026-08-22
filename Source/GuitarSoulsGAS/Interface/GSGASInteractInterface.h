// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSGASInteractInterface.generated.h"

// Blueprintable — BP 액터도 Class Settings에서 이 인터페이스를 구현할 수 있다.
UINTERFACE(MinimalAPI, Blueprintable)
class UGSGASInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 상호작용 대상이 구현하는 인터페이스.
 * C++ 구현체는 Interact_Implementation을 override하고, BP 구현체는 Interact 이벤트를 구현한다.
 * 호출 측은 반드시 Implements<UGSGASInteractInterface>() 확인 후 Execute_Interact()를 사용할 것.
 * (Cast<IGSGASInteractInterface>는 BP 구현체를 잡지 못한다.)
 */
class GUITARSOULSGAS_API IGSGASInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void Interact(AActor* InteractionActor);
};
