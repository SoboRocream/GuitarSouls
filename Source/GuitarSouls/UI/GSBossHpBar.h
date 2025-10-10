// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSUserWidget.h"
#include "Component/GSAttributeComponent.h"
#include "GSBossHpBar.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSBossHpBar : public UGSUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> HealthBarWidget;

public:
	void Init(UGSAttributeComponent* AttributeComponent);

protected:
	void OnAttributeChanged(EGSEnumDefine AttributeType, float InValue);
};
