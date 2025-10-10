// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSEquipment.h"
#include "GameFramework/Character.h"

// Sets default values
AGSEquipment::AGSEquipment()
{
 	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("EquipmentMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGSEquipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Mesh))
	{
		StaticMesh->SetStaticMesh(MeshAsset);
	}
}

void AGSEquipment::EquipItem()
{
}

void AGSEquipment::UnEquipItem()
{
}

void AGSEquipment::AttachToOwner(FName SocketName)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
		{
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
}

