// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_GSGASWCollision.h"

#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UAnimNotifyState_GSGASWCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Owner is null. NotifyBegin skipped."));
		return;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (!ASCInterface)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("%s does not implement IAbilitySystemInterface."), *Owner->GetName());
		return;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ASC is null on %s."), *Owner->GetName());
		return;
	}
 
	ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_AttackCollisionActive);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("AttackCollisionActive Added on %s."), *Owner->GetName());
}

void UAnimNotifyState_GSGASWCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!Owner)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Owner is null. NotifyBegin skipped."));
		return;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (!ASCInterface)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("%s does not implement IAbilitySystemInterface."), *Owner->GetName());
		return;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ASC is null on %s."), *Owner->GetName());
		return;
	}

	ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_AttackCollisionActive);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("AttackCollisionActive Removed on %s."), *Owner->GetName());
}

FString UAnimNotifyState_GSGASWCollision::GetNotifyName_Implementation() const
{
	return TEXT("GSGAS_WeaponCollision");
}
