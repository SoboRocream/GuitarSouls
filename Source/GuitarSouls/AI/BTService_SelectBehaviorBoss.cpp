// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_SelectBehaviorBoss.h"

#include "GSEnumDefine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSTestEnemy.h"
#include "Component/GSAttributeComponent.h"

void UBTService_SelectBehaviorBoss::UpdateBehaviorKey(UBlackboardComponent* BlackboardComp,
                                                      AGSTestEnemy* ControlledEnemy) const
{
	check(BlackboardComp);
	check(ControlledEnemy);

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	if (IsValid(TargetActor))
	{

		if (UGSAttributeComponent* AttributeComponent = ControlledEnemy->GetComponentByClass<UGSAttributeComponent>())
		{
			
			if (AttributeComponent->GetBaseStamina() <= StaminaCheckValue)
			{
				SetBehaviorKey(BlackboardComp, EGSAIBehavior::Strafe);
			}
			else
			{
				const float Distance = TargetActor->GetDistanceTo(ControlledEnemy);

				if (Distance <= AttackRangeDistance)
				{
					SetBehaviorKey(BlackboardComp, EGSAIBehavior::MeleeAttack);
				}
				else
				{
					SetBehaviorKey(BlackboardComp, EGSAIBehavior::Approach);
				}
			}
		}
	}
	else
	{
		SetBehaviorKey(BlackboardComp, EGSAIBehavior::Idle);
	}
}
