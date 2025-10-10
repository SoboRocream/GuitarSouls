// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSUserWidget.h"
#include "GSPlayerHUDWidget.generated.h"

enum class EGSEnumDefine : uint8;
/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSPlayerHUDWidget : public UGSUserWidget
{
	GENERATED_BODY()

public:
	UGSPlayerHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> StaminaBarWidget;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSStatBarWidget> HpBarWidget;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSPotionWidget> PotionWidget;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSWeaponWidget> ShieldWidget;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSWeaponWidget> WeaponWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTexture2D> BlankIcon;

protected:
	void OnAttributeChanged(EGSEnumDefine AttributeType, float InValue);

	void OnPotionQuantityChanged(uint8 InAmount);

	void OnWeaponChanged();
};
