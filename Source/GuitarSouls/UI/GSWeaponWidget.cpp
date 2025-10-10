// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSWeaponWidget.h"

#include "Components/Image.h"

void UGSWeaponWidget::SetWeaponImage(UTexture2D* InTextrue) const
{
	WeaponImage->SetBrushFromTexture(InTextrue);
}
