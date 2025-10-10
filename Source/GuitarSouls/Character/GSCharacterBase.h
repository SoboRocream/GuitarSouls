// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Default
};

UCLASS()
class GUITARSOULS_API AGSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGSCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UGSCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category=CharacterControl, meta=(AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UGSCharacterControlData*> CharacterControlManager;
};
