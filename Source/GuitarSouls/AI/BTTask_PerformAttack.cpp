// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_PerformAttack.h"

#include "AIController.h"
#include "Character/GSCharacterStateComponent.h"
#include "Interface/GSCombatInterface.h"

EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(ControlledPawn))
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindLambda([this, &OwnerComp, ControlledPawn](UAnimMontage* Montage, bool bInterrupted)
		{
			UE_LOG(LogTemp, Log, TEXT("Execute MontageEndedDelegate"));

			if (::IsValid(&OwnerComp) == false)
			{
				return;
			}
			if (UGSCharacterStateComponent* StateComponent = ControlledPawn->GetComponentByClass<UGSCharacterStateComponent>())
			{
				FGameplayTagContainer CheckTags;
				CheckTags.AddTag(GSGameplayTags::Character_State_Parried);
				CheckTags.AddTag(GSGameplayTags::Character_State_Stunned);

				if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false)
				{
					StateComponent->ClearState();
				}
				
			}

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		});

		CombatInterface->PerformAttack(AttackTypeTag, MontageEndedDelegate);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}
