// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTaskNode/BTTaskNode_GASStrafe.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GuitarSoulsGAS.h"

UBTTaskNode_GASStrafe::UBTTaskNode_GASStrafe()
{
	NodeName = TEXT("GAS Strafe");
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	TargetActorKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTaskNode_GASStrafe, TargetActorKey), AActor::StaticClass());
	StrafeDirectionKey.AddNameFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTaskNode_GASStrafe, StrafeDirectionKey));
}

FString UBTTaskNode_GASStrafe::GetStaticDescription() const
{
	return FString::Printf(TEXT("Radius: %.0f±%.0f | Angle: %.0f° | FlipChance: %.0f%%"),
		PreferredRadius, RadiusTolerance, StrafeAngleDeg, DirectionFlipChance * 100.f);
}

EBTNodeResult::Type UBTTaskNode_GASStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASStrafe: AIController is null."));
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASStrafe: Pawn is null."));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASStrafe: BlackboardComponent is null."));
		return EBTNodeResult::Failed;
	}

	const AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASStrafe: TargetActor is null."));
		return EBTNodeResult::Failed;
	}

	const FVector PawnLoc   = Pawn->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();

	// BB에서 현재 방향 읽기 — 미설정이면 Left 기본
	const FName CurrentDir = BB->GetValueAsName(StrafeDirectionKey.SelectedKeyName);
	bool bGoLeft = (CurrentDir != FName("Right"));

	// DirectionFlipChance 확률로 방향 전환
	if (FMath::FRand() < DirectionFlipChance)
	{
		bGoLeft = !bGoLeft;
	}

	// 결정된 방향 BB에 저장 (Task 간 유지)
	BB->SetValueAsName(StrafeDirectionKey.SelectedKeyName, bGoLeft ? FName("Left") : FName("Right"));

	CachedOwnerComp    = &OwnerComp;
	CachedAIController = AIController;
	bMoveStarted       = false;
	bCurrentGoLeft     = bGoLeft;
	RemainingSteps     = FMath::Max(0, StrafeStepCount - 1); // 첫 스텝은 아래에서 실행

	// 선택 방향 시도 → 실패 시 반대 방향 재시도
	if (TryMoveToStrafePoint(OwnerComp, PawnLoc, TargetLoc, bGoLeft))
	{
		return EBTNodeResult::InProgress;
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("GASStrafe: 1차 NavMesh 실패, 반대 방향 재시도."));
	BB->SetValueAsName(StrafeDirectionKey.SelectedKeyName, bGoLeft ? FName("Right") : FName("Left"));

	if (TryMoveToStrafePoint(OwnerComp, PawnLoc, TargetLoc, !bGoLeft))
	{
		return EBTNodeResult::InProgress;
	}

	GSGAS_LOG(LogGSGAS, Warning, TEXT("GASStrafe: 양방향 NavMesh 투영 실패. Task Failed."));
	return EBTNodeResult::Failed;
}

void UBTTaskNode_GASStrafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CachedAIController.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UPathFollowingComponent* PFC = CachedAIController->GetPathFollowingComponent();
	if (!PFC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// PFC가 아직 요청을 받아들이기 전(첫 틱)이면 대기
	if (!bMoveStarted)
	{
		if (PFC->GetCurrentRequestId() == MoveRequestID)
		{
			bMoveStarted = true;
		}
		return;
	}

	// 이동 시작 이후 요청 ID가 바뀌었으면 한 스텝 완료
	if (PFC->GetCurrentRequestId() != MoveRequestID)
	{
		if (RemainingSteps > 0)
		{
			// 다음 스텝: 현재 위치와 타겟 위치를 다시 조회해서 이동
			APawn* Pawn = CachedAIController->GetPawn();
			UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
			const AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;

			if (Pawn && Target)
			{
				RemainingSteps--;
				bMoveStarted = false;
				GSGAS_LOG(LogGSGAS, Log, TEXT("GASStrafe: 다음 스텝 실행 (남은 스텝: %d)"), RemainingSteps);

				if (!TryMoveToStrafePoint(OwnerComp, Pawn->GetActorLocation(), Target->GetActorLocation(), bCurrentGoLeft))
				{
					// 다음 스텝 NavMesh 실패 시 여기서 종료
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				}
				return;
			}
		}

		GSGAS_LOG(LogGSGAS, Log, TEXT("GASStrafe: 전체 이동 완료."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTaskNode_GASStrafe::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	MoveRequestID  = FAIRequestID::InvalidRequest;
	bMoveStarted   = false;
	RemainingSteps = 0;
	CachedOwnerComp.Reset();
	CachedAIController.Reset();
}

bool UBTTaskNode_GASStrafe::TryMoveToStrafePoint(UBehaviorTreeComponent& OwnerComp,
	const FVector& PawnLoc, const FVector& TargetLoc, bool bGoLeft)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld());
	if (!NavSys || !CachedAIController.IsValid()) return false;

	const FVector Desired = ComputeStrafeDestination(PawnLoc, TargetLoc, bGoLeft);

	FNavLocation NavLoc;
	if (!NavSys->ProjectPointToNavigation(Desired, NavLoc, FVector(50.f, 50.f, 200.f)))
	{
		return false;
	}

	// bCanStrafe=true: 측면 이동 허용
	const FAIRequestID RequestID = CachedAIController->MoveToLocation(
		NavLoc.Location, 20.f, /*bStopOnOverlap=*/true, /*bUsePathfinding=*/true,
		/*bProjectDestinationToNavigation=*/false, /*bCanStrafe=*/true);

	if (!RequestID.IsValid())
	{
		return false;
	}

	MoveRequestID = RequestID;
	return true;
}

FVector UBTTaskNode_GASStrafe::ComputeStrafeDestination(const FVector& PawnLoc, const FVector& TargetLoc, bool bGoLeft) const
{
	// 타겟→Pawn 방향 기준으로 StrafeAngleDeg 만큼 회전해 목적지 산출
	FVector TowardPawn = (PawnLoc - TargetLoc).GetSafeNormal2D();
	if (TowardPawn.IsNearlyZero())
	{
		TowardPawn = FVector::ForwardVector;
	}

	// bGoLeft=true → 반시계(왼쪽), false → 시계(오른쪽)
	const float AngleDeg  = bGoLeft ? -StrafeAngleDeg : StrafeAngleDeg;
	const FVector StrafeDir = TowardPawn.RotateAngleAxis(AngleDeg, FVector::UpVector);

	const float Radius = PreferredRadius + FMath::RandRange(-RadiusTolerance, RadiusTolerance);
	return TargetLoc + StrafeDir * Radius;
}
