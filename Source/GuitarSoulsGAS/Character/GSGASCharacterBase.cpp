// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSGASCharacterBase.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"


AGSGASCharacterBase::AGSGASCharacterBase()
{
	ASC = nullptr;
	WeaponCollision = CreateDefaultSubobject<UGSGASWeaponCollisionComponent>(TEXT("WeaponCollision"));
}

UAbilitySystemComponent* AGSGASCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

float AGSGASCharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// 레거시 적이 ApplyPointDamage를 쏠 때 GE로 변환하는 임시 브릿지.
	// 적/보스 GAS 전환 완료 후 제거.
	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ASC is null. TakeDamage bridge skipped."));
		return 0.f;
	}
 
	if (!TakeDamageBridgeEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("TakeDamageBridgeEffectClass is null. Set GSGE_Damage in Blueprint."));
		return 0.f;
	}
 
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddInstigator(EventInstigator ? EventInstigator->GetPawn() : DamageCauser, DamageCauser);
 
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		TakeDamageBridgeEffectClass, 1.f, ContextHandle);
 
	if (!SpecHandle.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("TakeDamage bridge SpecHandle is invalid."));
		return 0.f;
	}
 
	// GE_GSDamage는 Data.Damage SetByCaller 방식 — 레거시에서 넘어온 DamageAmount 주입
	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Data.Damage")), DamageAmount);
 
	const FActiveGameplayEffectHandle ActiveHandle =
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
 
	if (!ActiveHandle.WasSuccessfullyApplied())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("TakeDamage bridge GE failed to apply."));
		return 0.f;
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("TakeDamage bridge applied: %.1f on %s."), DamageAmount, *GetName());
 
	return DamageAmount;
}

void AGSGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();
 
	// 자신을 트레이스 무시 대상으로 등록
	WeaponCollision->AddIgnoreActor(this);
	
	WeaponCollision->SetWeaponMesh(GetMesh());
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("WeaponCollision initialized on %s."), *GetName());
}
