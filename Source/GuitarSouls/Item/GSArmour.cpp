// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSArmour.h"

#include "Component/GSAttributeComponent.h"
#include "Component/GSCombatComponent.h"
#include "GameFramework/Character.h"

AGSArmour::AGSArmour()
{
	if (USkeletalMeshComponent* SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmourMesh")))
	{
		Mesh->DestroyComponent();
		Mesh = SkeletalMesh;
		RootComponent = Mesh;
	}
}

void AGSArmour::EquipItem()
{
	Super::EquipItem();

	if (UGSCombatComponent* CombatComponent = GetOwner()->GetComponentByClass<UGSCombatComponent>())
	{
		CombatComponent->SetArmour(this);
		AttachToOwner(NAME_None);
	}

	if (UGSAttributeComponent* AttributeComponent = GetOwner()->GetComponentByClass<UGSAttributeComponent>())
	{
		AttributeComponent->IncreaseDefense(DefenseAmount);
	}
}

void AGSArmour::UnEquipItem()
{
	Super::UnEquipItem();

	if (UGSAttributeComponent* AttributeComponent = GetOwner()->GetComponentByClass<UGSAttributeComponent>())
	{
		AttributeComponent->DecreaseDefense(DefenseAmount);
	}

	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
	{
		SkeletalMesh->SetLeaderPoseComponent(nullptr);
	}

	// 현재 Transform 정보를 유지하면서 떼어낸다.
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

void AGSArmour::AttachToOwner(FName SocketName)
{
	Super::AttachToOwner(SocketName);

	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
	{
		if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			SkeletalMesh->SetLeaderPoseComponent(Character->GetMesh());
		}
	}
}
