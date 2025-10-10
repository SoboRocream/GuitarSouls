// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_GSStrafe.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_GSStrafe::UBTService_GSStrafe()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

// 서비스 활성화시 호출
void UBTService_GSStrafe::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return;
	}

	APawn* ControlledPawn = OwnerController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	if (AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName)))
	{
		OwnerController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
		if (const ACharacter* Character = Cast<ACharacter>(ControlledPawn))
		{
			if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
			{
				MovementComponent->bUseControllerDesiredRotation = true;
				MovementComponent->bOrientRotationToMovement = false;
			}
		}
	}
}

// 서비스 비활성화시 호출
void UBTService_GSStrafe::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return;
	}

	APawn* ControlledPawn = OwnerController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	OwnerController->ClearFocus(EAIFocusPriority::Gameplay);

	if (const ACharacter* Character = Cast<ACharacter>(ControlledPawn))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->bUseControllerDesiredRotation = false;
			MovementComponent->bOrientRotationToMovement = true;
		}
	}

	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UBTService_GSStrafe::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return;
	}

	APawn* ControlledPawn = OwnerController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const FVector Origin = ControlledPawn->GetActorLocation();
	FNavLocation OutRandomPoint(Origin);

	if (const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(ControlledPawn))
	{
		if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, OutRandomPoint))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerController, OutRandomPoint.Location);
		}
	}

}
