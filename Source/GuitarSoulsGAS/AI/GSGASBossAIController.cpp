// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/GSGASBossAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSGASCharacterBoss.h"
#include "GuitarSoulsGAS.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/GSGASGameplayTags.h"

AGSGASBossAIController::AGSGASBossAIController()
{
}

void AGSGASBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledBoss = Cast<AGSGASCharacterBoss>(InPawn);
	if (!ControlledBoss.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GSGASBossAIController: Possessed pawn is not AGSGASCharacterBoss."));
		return;
	}

	if (!BTAsset)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GSGASBossAIController: BTAsset not set on %s."), *GetName());
		return;
	}

	RunBehaviorTree(BTAsset);

	// OnPossess 시점에 플레이어가 아직 Possess되지 않았을 수 있으므로 한 프레임 뒤에 탐색
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AGSGASBossAIController::TryFindAndSetTarget);

	GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossAIController: BT started on %s."), *ControlledBoss->GetName());
}

void AGSGASBossAIController::OnUnPossess()
{
	// 플레이어 Death 태그 구독 해제
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Player))
		{
			if (UAbilitySystemComponent* PlayerASC = ASCInterface->GetAbilitySystemComponent())
			{
				PlayerASC->RegisterGameplayTagEvent(
					GSGASGameplayTags::Character_State_Death,
					EGameplayTagEventType::NewOrRemoved)
					.Remove(PlayerDeathTagHandle);
			}
		}
	}

	PlayerDeathTagHandle.Reset();
	SetTarget(nullptr);
	ControlledBoss.Reset();
	Super::OnUnPossess();
}

void AGSGASBossAIController::OnPlayerDeathTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossAIController: Player died, clearing target."));
		SetTarget(nullptr);
	}
}

void AGSGASBossAIController::TryFindAndSetTarget()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GSGASBossAIController: Player not found after tick delay."));
		return;
	}

	SetTarget(Player);

	// 플레이어 Death 태그 구독 — 사망 시 타겟 해제
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Player))
	{
		if (UAbilitySystemComponent* PlayerASC = ASCInterface->GetAbilitySystemComponent())
		{
			PlayerDeathTagHandle = PlayerASC->RegisterGameplayTagEvent(
				GSGASGameplayTags::Character_State_Death,
				EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &AGSGASBossAIController::OnPlayerDeathTagChanged);
		}
	}
}

void AGSGASBossAIController::SetTarget(AActor* InTarget)
{
	if (!Blackboard) return;

	Blackboard->SetValueAsObject(FName("TargetActor"), InTarget);

	if (!ControlledBoss.IsValid()) return;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(ControlledBoss.Get());
	if (!ASCInterface) return;

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC) return;

	if (InTarget)
	{
		if (!ASC->HasMatchingGameplayTag(GSGASGameplayTags::AI_State_Combat))
		{
			ASC->AddLooseGameplayTag(GSGASGameplayTags::AI_State_Combat);
			SetFocus(InTarget, EAIFocusPriority::Gameplay);
			GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossAIController: Combat started on %s."), *ControlledBoss->GetName());
		}
	}
	else
	{
		if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::AI_State_Combat))
		{
			ASC->RemoveLooseGameplayTag(GSGASGameplayTags::AI_State_Combat);
			ClearFocus(EAIFocusPriority::Gameplay);
			GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossAIController: Combat ended on %s."), *ControlledBoss->GetName());
		}
	}
}
