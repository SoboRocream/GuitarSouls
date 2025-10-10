// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GSEnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSCharacterPlayer.h"
#include "Character/GSTestEnemy.h"
#include "Component/GSRotationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

AGSEnemyAIController::AGSEnemyAIController()
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");
}

void AGSEnemyAIController::StopUpdateTarget()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	SetTarget(nullptr);
}

void AGSEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AGSTestEnemy>(InPawn);
	
	RunBehaviorTree(BTAsset);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGSEnemyAIController::UpdateTarget, 0.1f, true);
}

void AGSEnemyAIController::OnUnPossess()
{
	ControlledEnemy = nullptr;
	StopUpdateTarget();
	Super::OnUnPossess();
}

void AGSEnemyAIController::UpdateTarget() const
{
	TArray<AActor*> OutActors;
	Perception->GetKnownPerceivedActors(nullptr, OutActors);

	AGSCharacterPlayer* PlayerCharacter = Cast<AGSCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (OutActors.Contains(PlayerCharacter))
	{
		if (!PlayerCharacter->IsDeath())
		{
			SetTarget(PlayerCharacter);
			ControlledEnemy->ToggleHpBarVisibility(true);
			ControlledEnemy->WatchTarget(PlayerCharacter);
		}
		else
		{
			SetTarget(nullptr);
			ControlledEnemy->ToggleHpBarVisibility(false);
		}
	}
	else
	{
		SetTarget(nullptr);
		ControlledEnemy->ToggleHpBarVisibility(false);
	}
}

void AGSEnemyAIController::SetTarget(AActor* InTarget) const
{
	if (IsValid(Blackboard))
	{
		Blackboard->SetValueAsObject(FName("Target"), InTarget);
	}

	if (IsValid(ControlledEnemy))
	{
		if (UGSRotationComponent* RotationComponent = ControlledEnemy->GetComponentByClass<UGSRotationComponent>())
		{
			RotationComponent->SetTargetActor(InTarget);
		}
	}
	
}
