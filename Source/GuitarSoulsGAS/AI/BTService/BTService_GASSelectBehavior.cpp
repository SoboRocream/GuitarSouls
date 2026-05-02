// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_GASSelectBehavior.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSGASCharacterEnemy.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"

UBTService_GASSelectBehavior::UBTService_GASSelectBehavior()
{
	NodeName = TEXT("GAS Select Behavior");
}

void UBTService_GASSelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
 
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}
 
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASCInterface)
	{
		return;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
 
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}
 
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
 
	FGameplayTag NewTag;
 
	if (Target)
	{
		const float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
		NewTag = (Distance <= AttackRange)
			? GSGASGameplayTags::AI_Behavior_Strafe
			: GSGASGameplayTags::AI_Behavior_Approach;
	}
	else
	{
		const AGSGASCharacterEnemy* Enemy = Cast<AGSGASCharacterEnemy>(Pawn);
		const bool bHasPatrol = Enemy && Enemy->GetCurrentPatrolPoint() != nullptr;
		NewTag = bHasPatrol
			? GSGASGameplayTags::AI_Behavior_Patrol
			: GSGASGameplayTags::AI_Behavior_Idle;
	}
 
	if (NewTag != CurrentBehaviorTag)
	{
		SetBehaviorTag(ASC, NewTag);
	}
}

void UBTService_GASSelectBehavior::SetBehaviorTag(UAbilitySystemComponent* ASC, const FGameplayTag& NewTag)
{
	if (CurrentBehaviorTag.IsValid())
	{
		ASC->RemoveLooseGameplayTag(CurrentBehaviorTag);
	}
 
	ASC->AddLooseGameplayTag(NewTag);
	CurrentBehaviorTag = NewTag;
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTService_GASSelectBehavior: Behavior → %s"), *NewTag.ToString());

}
