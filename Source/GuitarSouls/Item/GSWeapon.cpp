// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSWeapon.h"
#include "Data/GSMontageActionData.h"
#include "Component/GSCombatComponent.h"
#include "Component/GSWeaponCollisionComponent.h"
#include "Tag/GSGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/GSAnimInstance.h"
#include "Character/GSCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"

AGSWeapon::AGSWeapon()
{
	MainWeaponCollision = CreateDefaultSubobject<UGSWeaponCollisionComponent>(TEXT("MainWeaponCollision"));
	MainWeaponCollision->OnHitActor.AddUObject(this, &AGSWeapon::OnHitActor);

	SecondWeaponCollision = CreateDefaultSubobject<UGSWeaponCollisionComponent>(TEXT("SecondWeaponCollision"));
	SecondWeaponCollision->OnHitActor.AddUObject(this, &AGSWeapon::OnHitActor);
	
	StaminaCostMap.Add(GSGameplayTags::Character_Attack_Light, 7.f);
	StaminaCostMap.Add(GSGameplayTags::Character_Attack_Heavy, 12.f);
	StaminaCostMap.Add(GSGameplayTags::Character_Attack_Running, 15.f);
	StaminaCostMap.Add(GSGameplayTags::Character_Attack_Special, 20.f);

	DamageMultiplierMap.Add(GSGameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(GSGameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(GSGameplayTags::Character_Attack_Special, 2.1f);
}

void AGSWeapon::EquipItem()
{
	Super::EquipItem();
	CombatComponent = GetOwner()->GetComponentByClass<UGSCombatComponent>();
	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);

		const FName AttachSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnEquipSocketName;

		AttachToOwner(AttachSocket);

		// 무기의 충돌 트레이스 컴포넌트에 무기 메쉬 컴포넌트를 설정
		MainWeaponCollision->SetMeshWeapon(Mesh);
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (UGSAnimInstance* Anim = Cast<UGSAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}
		}

		MainWeaponCollision->AddIgnoreActor(GetOwner());

		if (AGSShield* Shield = CombatComponent->GetShield())
		{
			FName ShieldSocket = Shield->GetUnequipSocketName();
			if (CombatType == ECombatType::SwordShield)
			{
				if (CombatComponent->IsCombatEnabled())
				{
					ShieldSocket = Shield->GetEquipSocketName();
				}
			}

			Shield->AttachToOwner(ShieldSocket);
		}
	}
}

void AGSWeapon::Drop()
{
	if (CombatType != ECombatType::MeleeFists)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
	}
}

UAnimMontage* AGSWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

UAnimMontage* AGSWeapon::GetRandomMontageForTag(const FGameplayTag& Tag) const
{
	return MontageActionData->GetRandomMontageForTag(Tag);
}

UAnimMontage* AGSWeapon::GetHitReactMontage(const AActor* Attacker) const
{
	// LookAt 회전값 (현재 Actor가 공격자를 바라보는 회전값
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation());
	// 현재 Actor의 회전값과 LookAt 회전값의 차이
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), LookAtRotation);
	// Z축 기준의 회전값 차이
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f))
	{
		HitDirection = EHitDirection::Front;
		UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f))
	{
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f) || UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, 135.f))
	{
		HitDirection = EHitDirection::Back;
		UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f))
	{
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}

	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection)
	{
	case EHitDirection::Front:
		SelectedMontage = GetMontageForTag(GSGameplayTags::Character_Action_HitReaction, 0);
		break;
	case EHitDirection::Left:
		SelectedMontage = GetMontageForTag(GSGameplayTags::Character_Action_HitReaction, 1);
		break;
	case EHitDirection::Right:
		SelectedMontage = GetMontageForTag(GSGameplayTags::Character_Action_HitReaction, 2);
		break;
	case EHitDirection::Back:
		SelectedMontage = GetMontageForTag(GSGameplayTags::Character_Action_HitReaction, 3);
		break;
		
	}

	return SelectedMontage;
}

float AGSWeapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag))
	{
		return StaminaCostMap[InTag];
	}
	
	return 0.0f;
}

float AGSWeapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType();
		if (DamageMultiplierMap.Contains(LastAttackType))
		{
			const float Multiples = DamageMultiplierMap[LastAttackType];
			return BaseDamage * Multiples;
		}
	}

	return BaseDamage;
}

void AGSWeapon::ActivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::MainCollision:
		MainWeaponCollision->TurnOnCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOnCollision();
		break;
	}
}

void AGSWeapon::DeactivateCollision(EWeaponCollisionType InCollisionType)
{
	switch (InCollisionType)
	{
	case EWeaponCollisionType::MainCollision:
		MainWeaponCollision->TurnOffCollision();
		break;
	case EWeaponCollisionType::SecondCollision:
		SecondWeaponCollision->TurnOffCollision();
		break;
	}
}

void AGSWeapon::OnHitActor(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();

	FVector DamageDirection = GetOwner()->GetActorForwardVector();

	float AttackDamage = GetAttackDamage();

	UGameplayStatics::ApplyPointDamage(
		HitActor,
		AttackDamage,
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr);

	UE_LOG(LogTemp, Log, TEXT("AGSWeapon::OnHitActor(const FHitResult& Hit) Called"))
}

// void AGSWeapon::UnEquipItem()
// {
// 	Super::UnEquipItem();
// }
