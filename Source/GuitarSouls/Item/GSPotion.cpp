// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GSPotion.h"

AGSPotion::AGSPotion()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

