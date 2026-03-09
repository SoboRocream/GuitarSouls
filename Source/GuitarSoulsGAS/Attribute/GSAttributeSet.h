// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE(FOnOutOfHealth);
DECLARE_MULTICAST_DELEGATE(FOnOutOfStamina);
DECLARE_MULTICAST_DELEGATE(FOnOutOfPotion);

UCLASS()
class GUITARSOULSGAS_API UGSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UGSAttributeSet();

	ATTRIBUTE_ACCESSORS(UGSAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, PotionCount);
	ATTRIBUTE_ACCESSORS(UGSAttributeSet, MaxPotionCount);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	//Health
	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxHealth;

	//Stamina
	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxStamina;

	//Damage
	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

	// Potion
	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData PotionCount;

	UPROPERTY(BlueprintReadOnly, Category=Attribute, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxPotionCount;

public:
	FOnOutOfHealth OnOutOfHealth;
	FOnOutOfStamina OnOutOfStamina;
	FOnOutOfPotion OnOutOfPotion;
};
