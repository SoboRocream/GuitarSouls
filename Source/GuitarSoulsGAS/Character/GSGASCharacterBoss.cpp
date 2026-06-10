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
#include "UI/GSGASVictoryWidget.h"

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

	// AvatarActor 정보는 컨트롤러 교체 시에도 갱신이 필요하므로 가드 밖에 위치
	ASC->InitAbilityActorInfo(this, this);

	// 이미 초기화됐으면 델리게이트 이중 바인딩 / 어빌리티 이중 부여 / 무기 이중 스폰 방지
	if (bGASInitialized)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("PossessedBy called again on %s, skip GAS setup."), *GetName());
		return;
	}
	bGASInitialized = true;

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

	for (const TSubclassOf<UGameplayAbility>& Ability : AttackAbilities)
	{
		if (Ability)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(Ability));
		}
	}

	SpawnAndEquipWeaponInCombat(DefaultWeaponClass);

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
	// Victory 위젯은 BP OnDeath 오버라이드와 무관하게 항상 실행되어야 하므로 여기서 처리
	// BindWeakLambda(GetWorld(), ...) — 콜백 생존 여부를 World 기준으로 판단하므로
	// 액터가 Destroy된 이후에도 타이머가 취소되지 않음
	if (VictoryWidgetClass)
	{
		TSubclassOf<UGSGASVictoryWidget> CachedClass = VictoryWidgetClass;
		FTimerDelegate Delegate;
		Delegate.BindWeakLambda(GetWorld(), [this, CachedClass]()
		{
			ShowVictoryWidget(CachedClass);
		});
		GetWorldTimerManager().SetTimer(VictoryTimerHandle, Delegate, VictoryWidgetDelay, false);
	}

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

void AGSGASCharacterBoss::ShowVictoryWidget(TSubclassOf<UGSGASVictoryWidget> WidgetClass)
{
	if (!WidgetClass) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	UGSGASVictoryWidget* VictoryWidget = CreateWidget<UGSGASVictoryWidget>(PC, WidgetClass);
	if (VictoryWidget)
	{
		VictoryWidget->AddToViewport(10);
	}
}
