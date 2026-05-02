// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/GSGASEnemyAIController.h"

#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterEnemy.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/GSGASGameplayTags.h"

AGSGASEnemyAIController::AGSGASEnemyAIController()
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
 
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
 
	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(SightConfig->GetSenseImplementation());
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AGSGASEnemyAIController::OnTargetPerceptionUpdated);
}

void AGSGASEnemyAIController::SetAlertTarget(AActor* InInstigator)
{
	// 이미 전투 중이면 무시
	if (!ControlledEnemy.IsValid())
	{
		return;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(ControlledEnemy.Get());
	if (!ASCInterface)
	{
		return;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (ASC && ASC->HasMatchingGameplayTag(GSGASGameplayTags::AI_State_Combat))
	{
		return;
	}
 
	SetTarget(InInstigator);
	GSGAS_LOG(LogGSGAS, Log, TEXT("AGSGASEnemyAIController: Alert received, target set on %s."), *ControlledEnemy->GetName());
}

void AGSGASEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
 
	ControlledEnemy = Cast<AGSGASCharacterEnemy>(InPawn);
	if (!ControlledEnemy.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AGSGASEnemyAIController: Possessed pawn is not AGSGASCharacterEnemy."));
		return;
	}
 
	if (!BTAsset)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AGSGASEnemyAIController: BTAsset is not set on %s."), *GetName());
		return;
	}
 
	RunBehaviorTree(BTAsset);
	GSGAS_LOG(LogGSGAS, Log, TEXT("AGSGASEnemyAIController: BT started on %s."), *ControlledEnemy->GetName());
}

void AGSGASEnemyAIController::OnUnPossess()
{
	SetTarget(nullptr);
	ControlledEnemy.Reset();
	Super::OnUnPossess();
}

void AGSGASEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 플레이어인지 확인
	if (Actor != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		return;
	}
 
	// 사망 체크
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (TargetASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_Death))
			{
				SetTarget(nullptr);
				return;
			}
		}
	}
 
	if (Stimulus.WasSuccessfullySensed())
	{
		SetTarget(Actor);
		PropagateAlert(Actor);
	}
	else
	{
		SetTarget(nullptr);
	}
}

void AGSGASEnemyAIController::SetTarget(AActor* InTarget)
{
	if (!Blackboard)
	{
		return;
	}
 
	Blackboard->SetValueAsObject(FName("TargetActor"), InTarget);
 
	// ASC State 태그 관리
	if (!ControlledEnemy.IsValid())
	{
		return;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(ControlledEnemy.Get());
	if (!ASCInterface)
	{
		return;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
 
	if (InTarget)
	{
		if (!ASC->HasMatchingGameplayTag(GSGASGameplayTags::AI_State_Combat))
		{
			ASC->AddLooseGameplayTag(GSGASGameplayTags::AI_State_Combat);
			SetFocus(InTarget, EAIFocusPriority::Gameplay);
			GSGAS_LOG(LogGSGAS, Log, TEXT("AGSGASEnemyAIController: Combat started on %s."), *ControlledEnemy->GetName());
		}
	}
	else
	{
		if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::AI_State_Combat))
		{
			ASC->RemoveLooseGameplayTag(GSGASGameplayTags::AI_State_Combat);
			ClearFocus(EAIFocusPriority::Gameplay);
			GSGAS_LOG(LogGSGAS, Log, TEXT("AGSGASEnemyAIController: Combat ended on %s."), *ControlledEnemy->GetName());
		}
	}
}

void AGSGASEnemyAIController::PropagateAlert(AActor* InInstigator)
{
	if (!ControlledEnemy.IsValid())
	{
		return;
	}
 
	const float AlertRadius = ControlledEnemy->AlertRadius;
	const FVector Origin = ControlledEnemy->GetActorLocation();
 
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledEnemy.Get());
 
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AlertRadius),
		Params
	);
 
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (AGSGASCharacterEnemy* NearbyEnemy = Cast<AGSGASCharacterEnemy>(Result.GetActor()))
		{
			NearbyEnemy->NotifyAlert(InInstigator);
		}
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("AGSGASEnemyAIController: Alert propagated from %s."), *ControlledEnemy->GetName());
}
