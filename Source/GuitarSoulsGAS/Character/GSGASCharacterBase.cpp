// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSGASCharacterBase.h"
#include "Item/GSGASWeapon.h"
#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"


AGSGASCharacterBase::AGSGASCharacterBase()
{
	ASC = nullptr;
}

UAbilitySystemComponent* AGSGASCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AGSGASCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (ASC)
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

void AGSGASCharacterBase::SetEquippedWeapon(class AGSGASWeapon* InWeapon)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Drop();
	}
	EquippedWeapon = InWeapon;
	GSGAS_LOG(LogGSGAS, Log, TEXT("EquippedWeapon set on %s."), *GetName());
}

void AGSGASCharacterBase::SetCombatEnabled(bool bEnabled)
{
	bCombatEnabled = bEnabled;
}

void AGSGASCharacterBase::DisableEquippedWeaponPhysics()
{
	if (!EquippedWeapon) return;

	if (UStaticMeshComponent* WeaponMesh = EquippedWeapon->GetWeaponMesh())
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
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
	GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASCharacterBase BeginPlay on %s."), *GetName());
}

void AGSGASCharacterBase::SpawnAndEquipWeaponInCombat(TSubclassOf<AGSGASWeapon> WeaponClass)
{
	SpawnAndEquipWeapon(WeaponClass, true);
}

void AGSGASCharacterBase::SpawnAndEquipWeapon(TSubclassOf<AGSGASWeapon> WeaponClass, bool bEnterCombat)
{
	if (!WeaponClass)
	{
		return;
	}

	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("SpawnAndEquipWeapon: ASC not initialized on %s."), *GetName());
		return;
	}

	// 이미 무기가 장착됐으면 스킵 — bGASInitialized 가드가 있어야 여기 도달하지 않지만 이중 방어
	if (EquippedWeapon)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("SpawnAndEquipWeapon: already equipped on %s, skip."), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGSGASWeapon* Weapon = GetWorld()->SpawnActor<AGSGASWeapon>(WeaponClass, GetActorTransform(), SpawnParams);
	if (!Weapon)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("SpawnAndEquipWeapon: SpawnActor failed on %s."), *GetName());
		return;
	}

	// UnEquipSocket(등)에 붙이고 EquippedWeapon 설정
	Weapon->EquipItem();

	if (bEnterCombat)
	{
		// 몽타주 없이 즉시 손(EquipSocket)으로 이동
		Weapon->SwitchSocket(true);

		// 전투 상태 플래그 + GAS 태그 적용
		SetCombatEnabled(true);
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_CombatEnabled);
		if (Weapon->GetWeaponTypeTag().IsValid())
		{
			ASC->AddLooseGameplayTag(Weapon->GetWeaponTypeTag());
		}
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("SpawnAndEquipWeapon: equipped on %s (combat=%d)."), *GetName(), bEnterCombat ? 1 : 0);
}

void AGSGASCharacterBase::DestroyEquippedWeapon()
{
	if (EquippedWeapon)
	{
		// 전투 중 무기 타입 태그가 붙어 있었다면 제거
		if (ASC && EquippedWeapon->GetWeaponTypeTag().IsValid())
		{
			ASC->RemoveLooseGameplayTag(EquippedWeapon->GetWeaponTypeTag());
		}

		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	SetCombatEnabled(false);
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_CombatEnabled);
	}
}
