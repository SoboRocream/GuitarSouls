#include "AnimNotifyState_GSWeaponCollision.h"

#include "Component/GSCombatComponent.h"
#include "Component/GSWeaponCollisionComponent.h"
#include "Interface/GSCombatInterface.h"
#include "Item/GSWeapon.h"

UAnimNotifyState_GSWeaponCollision::UAnimNotifyState_GSWeaponCollision(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAnimNotifyState_GSWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(OwnerActor))
		{
			CombatInterface->ActivateWeaponCollision(CollisionType);
		}
	}
}

void UAnimNotifyState_GSWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(OwnerActor))
		{
			CombatInterface->DeactivateWeaponCollision(CollisionType);
		}
	}
}


