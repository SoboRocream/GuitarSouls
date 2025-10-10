// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_GSKnockBack.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UAnimNotify_GSKnockBack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const FVector OriginLocation = MeshComp->GetSocketLocation(LocationSocketName);

	if (bDrawDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(MeshComp, OriginLocation, DamageRadius, 12, FLinearColor::White, 2.f, 1.f);
	}

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController)
	{
		return;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerPawn);

	UGameplayStatics::ApplyRadialDamage(OwnerPawn, BaseDamage, OriginLocation, DamageRadius, nullptr, ActorsToIgnore, nullptr, OwnerController, false, ECC_Visibility);
	
	
}
