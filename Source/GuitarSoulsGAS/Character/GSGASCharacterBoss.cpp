// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GSGASCharacterBoss.h"

#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"
#include "Attribute/GSAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/GSGASCollision.h"
#include "Tags/GSGASGameplayTags.h"
#include "UI/GSGASBossHpBar.h"

AGSGASCharacterBoss::AGSGASCharacterBoss()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UGSAttributeSet>(TEXT("AttributeSet"));

	// 락온 타겟팅 스피어
	TargetingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphere->SetupAttachment(RootComponent);
	TargetingSphere->SetSphereRadius(50.f);
	TargetingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetingSphere->SetCollisionObjectType(CCHANNEL_GSGAS_TARGETING);
	TargetingSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidgetComponent->SetupAttachment(RootComponent);
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);
	LockOnWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGSGASCharacterBoss::OnTargeted(bool bTargeted)
{
	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
}

void AGSGASCharacterBoss::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Error, TEXT("ASC is null on %s."), *GetName());
		return;
	}

	ASC->InitAbilityActorInfo(this, this);

	if (AttributeSet)
	{
		AttributeSet->OnOutOfHealth.AddUObject(this, &AGSGASCharacterBoss::OnOutOfHealth);

		ASC->GetGameplayAttributeValueChangeDelegate(UGSAttributeSet::GetHealthAttribute())
			.AddUObject(this, &AGSGASCharacterBoss::OnHealthChanged);
	}

	if (InitStatEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitStatEffectClass, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else
		{
			GSGAS_LOG(LogGSGAS, Warning, TEXT("InitStatEffectClass SpecHandle invalid on %s."), *GetName());
		}
	}

	for (const TSubclassOf<UGameplayAbility>& Ability : StartAbilities)
	{
		if (Ability)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(Ability));
		}
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("Boss ASC initialized on %s."), *GetName());
}

void AGSGASCharacterBoss::BeginPlay()
{
	Super::BeginPlay();

	if (!BossHpBarWidgetClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BossHpBarWidgetClass not set on %s."), *GetName());
		return;
	}

	BossHpBarWidget = CreateWidget<UGSGASBossHpBar>(GetWorld(), BossHpBarWidgetClass);
	if (!BossHpBarWidget)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BossHpBar CreateWidget failed on %s."), *GetName());
		return;
	}

	BossHpBarWidget->AddToViewport();
	BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);

	// PossessedBy는 BeginPlay 이전에 호출되므로 여기서 바인딩
	if (ASC)
	{
		BossHpBarWidget->SetAbilitySystemComponent(this);
		BossHpBarWidget->SetBossName(BossName);
		GSGAS_LOG(LogGSGAS, Log, TEXT("BossHpBar bound on %s."), *GetName());
	}
	else
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ASC null at BeginPlay on %s. HpBar not bound."), *GetName());
	}
}

void AGSGASCharacterBoss::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (bPhaseTransitionTriggered) return;
	if (!ASC) return;

	if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::Boss_Phase_2)) return;

	if (Data.NewValue <= Phase2HPThreshold)
	{
		bPhaseTransitionTriggered = true;

		if (PhaseTransitionAbilityClass)
		{
			ASC->TryActivateAbilityByClass(PhaseTransitionAbilityClass);
			GSGAS_LOG(LogGSGAS, Log, TEXT("Phase transition triggered on %s. (HP: %.1f)"), *GetName(), Data.NewValue);
		}
		else
		{
			GSGAS_LOG(LogGSGAS, Warning, TEXT("PhaseTransitionAbilityClass not set on %s."), *GetName());
		}
	}
}

void AGSGASCharacterBoss::OnOutOfHealth()
{
	OnDeath();
}

void AGSGASCharacterBoss::OnDeath_Implementation()
{
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_Death);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (TargetingSphere)
	{
		TargetingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (BossHpBarWidget)
	{
		BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("Boss %s died."), *GetName());
}
