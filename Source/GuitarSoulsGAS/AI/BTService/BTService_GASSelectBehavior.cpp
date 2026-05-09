// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_GASSelectBehavior.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSGASCharacterEnemy.h"
#include "GuitarSoulsGAS.h"

UBTService_GASSelectBehavior::UBTService_GASSelectBehavior()
{
	NodeName = TEXT("Select Behavior");

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GASSelectBehavior, TargetActorKey), AActor::StaticClass());
	BehaviorStateKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GASSelectBehavior, BehaviorStateKey));
}

FString UBTService_GASSelectBehavior::GetStaticDescription() const
{
	return FString::Printf(TEXT("AttackRange: %.1f | State → %s"),
		AttackRange, *BehaviorStateKey.SelectedKeyName.ToString());
}

void UBTService_GASSelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

	FName NewState;
	if (Target)
	{
		const float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
		NewState = (Distance <= AttackRange) ? FName("Strafe") : FName("Approach");
	}
	else
	{
		const AGSGASCharacterEnemy* Enemy = Cast<AGSGASCharacterEnemy>(Pawn);
		NewState = (Enemy && Enemy->GetCurrentPatrolPoint()) ? FName("Patrol") : FName("Idle");
	}

	BB->SetValueAsName(BehaviorStateKey.SelectedKeyName, NewState);
	GSGAS_LOG(LogGSGAS, Log, TEXT("SelectBehavior: %s"), *NewState.ToString());
}
