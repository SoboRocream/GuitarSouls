// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSTestBoss.h"

#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GSBossHpBar.h"
#include "UI/GSStatBarWidget.h"

void AGSTestBoss::BeginPlay()
{
	Super::BeginPlay();
	if (HpBarWidgetComponent)
	{
		HpBarWidgetComponent->DestroyComponent();
	}

	if (BossHealthBarWidgetClass)
	{
		BossHpBarWidget = CreateWidget<UGSBossHpBar>(GetWorld(), BossHealthBarWidgetClass);
		if (BossHpBarWidget)
		{
			BossHpBarWidget->Init(AttributeComponent);
			BossHpBarWidget->AddToViewport();
			BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AGSTestBoss::OnDeath()
{
	Super::OnDeath();

	if (BossHpBarWidget)
	{
		BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(BossMusic))
	{
		BossMusic->FadeOut(2.f, 0);
	}
}

void AGSTestBoss::HitReaction(const AActor* Attacker)
{
}

void AGSTestBoss::WatchTarget(AActor* InTargetActor)
{
	if (IsValid(InTargetActor))
	{
		if (BossHpBarWidget)
		{
			BossHpBarWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}

	if (BossMusicAsset)
	{
		if (!bStartedBossMusic)
		{
			bStartedBossMusic = true;
			BossMusic = UGameplayStatics::SpawnSound2D(this, BossMusicAsset);
			BossMusic->FadeIn(1.f);
		}
	}
}

void AGSTestBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Cyan, FString::Printf(TEXT("%f"), AttributeComponent->GetBaseStamina()));
}
