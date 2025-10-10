// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSUserWidget.h"
#include "GSWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSWeaponWidget : public UGSUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> WeaponImage;

public:
	void SetWeaponImage(UTexture2D* InTextrue) const;
};
