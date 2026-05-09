// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService/BTService_GSGASUpdateLastKnown.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_GSGASUpdateLastKnown::UBTService_GSGASUpdateLastKnown()
{
	NodeName = TEXT("Update Last Known Location");

	TargetActorKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTService_GSGASUpdateLastKnown, TargetActorKey),
		AActor::StaticClass());

	LastKnownLocationKey.AddVectorFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTService_GSGASUpdateLastKnown, LastKnownLocationKey));
}

FString UBTService_GSGASUpdateLastKnown::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s → %s"),
		*TargetActorKey.SelectedKeyName.ToString(),
		*LastKnownLocationKey.SelectedKeyName.ToString());
}

void UBTService_GSGASUpdateLastKnown::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	const AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target) return;

	BB->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, Target->GetActorLocation());
}
