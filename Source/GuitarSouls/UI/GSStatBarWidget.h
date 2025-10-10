// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSUserWidget.h"
#include "GSStatBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSStatBarWidget : public UGSUserWidget
{
	GENERATED_BODY()

public:
	UGSStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void SetRatio(float Ratio) const;

protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UProgressBar> StatBar;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatBar")
	FLinearColor FillColorAndOpacity = FLinearColor::Red;
	
};
