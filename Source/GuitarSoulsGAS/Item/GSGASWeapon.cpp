// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSGASWeapon.h"
#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterBase.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "Data/GSGASWeaponData.h"
#include "GameFramework/Character.h"
#include "Data/GSGASCollision.h"
#include "Components/SphereComponent.h"

// Sets default values
AGSGASWeapon::AGSGASWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
 
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	// 상호작용 감지용 구체 컴포넌트
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Mesh);
	InteractionSphere->SetSphereRadius(100.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(CCHANNEL_GSGAS_INTERACTION);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Main 콜리전 (Index 0)
	MainWeaponCollision = CreateDefaultSubobject<UGSGASWeaponCollisionComponent>(TEXT("MainWeaponCollision"));
}

void AGSGASWeapon::Interact(AActor* InteractionActor)
{
	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(InteractionActor);
	if (!Character)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Interact: InteractionActor is not AGSGASCharacterBase."));
		return;
	}
 
	SetOwner(Character);
	EquipItem();
}

void AGSGASWeapon::EquipItem()
{
	AGSGASCharacterBase* OwnerCharacter = Cast<AGSGASCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("EquipItem: Owner is not AGSGASCharacterBase."));
		return;
	}
	
	OwnerCharacter->SetEquippedWeapon(this);
 
	// Main 콜리전 초기화
	if (UGSGASWeaponCollisionComponent* MainCollision = GetWeaponCollision())
	{
		MainCollision->SetWeaponMesh(Mesh);
		MainCollision->AddIgnoreActor(OwnerCharacter);
	}
	
	AttachToOwnerCharacter(EquipSocketName);
 
	// 물리/콜리전 비활성 (픽업 상태 → 장착 상태)
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("EquipItem on %s."), *OwnerCharacter->GetName());
}

void AGSGASWeapon::Drop()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
 
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
 
	SetOwner(nullptr);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("Weapon dropped."));
}

UAnimMontage* AGSGASWeapon::GetMontageForTag(const FGameplayTag& Tag) const
{
	if (!WeaponData)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponData is null."));
		return nullptr;
	}
	return WeaponData->GetMontageForTag(Tag);
}

float AGSGASWeapon::GetStaminaCost(const FGameplayTag& Tag) const
{
	if (!WeaponData)
	{
		return 0.f;
	}
	return WeaponData->GetStaminaCost(Tag);
}

float AGSGASWeapon::GetFinalDamage(const FGameplayTag& Tag) const
{
	if (!WeaponData)
	{
		return 0.f;
	}
	return WeaponData->GetFinalDamage(Tag);
}

void AGSGASWeapon::AttachToOwnerCharacter(FName SocketName)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}
 
	AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SocketName);
}


