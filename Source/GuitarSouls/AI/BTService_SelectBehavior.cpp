// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_SelectBehavior.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSCharacterStateComponent.h"
#include "Character/GSTestEnemy.h"
#include "Tag/GSGameplayTags.h"

UBTService_SelectBehavior::UBTService_SelectBehavior()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	// bCreateNodeInstance = true; //캐릭터 별로 노드를 따로 생성함 -> 성능 저하 발생 가능
	// 일반적으로는 블랙보드에서 값 처리해야함
}

void UBTService_SelectBehavior::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	
}

void UBTService_SelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//일반적으로는 블랙보드에서 처리해야 함.
	if (APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn())
		if (AGSTestEnemy* ControlledEnemy = Cast<AGSTestEnemy>(ControlledPawn))
		{
			UpdateBehaviorKey(OwnerComp.GetBlackboardComponent(), ControlledEnemy);
		}

}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, EGSAIBehavior Behavior) const
{
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::UpdateBehaviorKey(UBlackboardComponent* BlackboardComp, AGSTestEnemy* ControlledEnemy) const
{
	check(BlackboardComp);
	check(ControlledEnemy);

	const UGSCharacterStateComponent* StateComponent = ControlledEnemy->GetComponentByClass<UGSCharacterStateComponent>();
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Parried);
	CheckTags.AddTag(GSGameplayTags::Character_State_Stunned);

	if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
	{
		SetBehaviorKey(BlackboardComp, EGSAIBehavior::Stunned);
	}
	else
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));

		if (IsValid(TargetActor))
		{
			const float Distance = TargetActor->GetDistanceTo(ControlledEnemy);

			// In Attack Range
			if (Distance <= AttackRangeDistance)
			{
				SetBehaviorKey(BlackboardComp, EGSAIBehavior::MeleeAttack);
			}
			else
			{
				SetBehaviorKey(BlackboardComp, EGSAIBehavior::Approach);
			}
		}
		else
		{
			// has Patrol
			if (ControlledEnemy->GetTargetPoint() != nullptr)
			{
				SetBehaviorKey(BlackboardComp, EGSAIBehavior::Patrol);
			}
			else
			{
				SetBehaviorKey(BlackboardComp, EGSAIBehavior::Idle);
			}
		}
	}
	
}
