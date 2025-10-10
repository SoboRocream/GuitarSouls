

#include "Item/GSPickUpItem.h"
#include "Physics//GSCollision.h"
#include "GSEquipment.h"

AGSPickUpItem::AGSPickUpItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpItemMesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionObjectType(CCHANNEL_GSINTERACTION);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AGSPickUpItem::Interact(AActor* InteractionActor)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InteractionActor;

	AGSEquipment* SpawnItem = GetWorld()->SpawnActor<AGSEquipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem)
	{
		SpawnItem->EquipItem();
		Destroy();
	}
}

void AGSPickUpItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (EquipmentClass)
	{
		if (AGSEquipment* CDO = EquipmentClass->GetDefaultObject<AGSEquipment>())
		{
			Mesh->SetStaticMesh(CDO->MeshAsset);
			Mesh->SetSimulatePhysics(true);
		}
	}
}
