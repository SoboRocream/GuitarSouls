// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSGASCharacterEnemy.h"

#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"
#include "Attribute/GSAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/GSGASCollision.h"
#include "Tags/GSGASGameplayTags.h"
#include "UI/GSGASUserWidget.h"

AGSGASCharacterEnemy::AGSGASCharacterEnemy()
{
	// CharacterBase -> ASC 
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	AttributeSet = CreateDefaultSubobject<UGSAttributeSet>(TEXT("AttributeSet"));

	// LockOn Targeting Sphere
	// Visibility Channel Block -> GSGASTargetingComponent -> SphereTrace Check
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

	HpBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HpBarWidgetComponent->SetupAttachment(GetMesh());
	HpBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HpBarWidgetComponent->SetDrawSize(FVector2D(100.f, 5.f));
	HpBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HpBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HpBarWidgetComponent->SetVisibility(false);

}

void AGSGASCharacterEnemy::OnTargeted(bool bTargeted)
{
	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
	if (HpBarWidgetComponent)
	{
		HpBarWidgetComponent->SetVisibility(true);
	}
}

void AGSGASCharacterEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Error, TEXT("ASC is null on %s. Check Constructor."), *GetName());
		return;
	}

	ASC->InitAbilityActorInfo(this, this);

	// Death Delegate Bind
	if (AttributeSet)
	{
		AttributeSet->OnOutOfHealth.AddUObject(this, &AGSGASCharacterEnemy::OnOutOfHealth);
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
			{
				GSGAS_LOG(LogGSGAS, Warning, TEXT("InitStatEffectClass SpecHandle invalid on %s."), *GetName());
			}
		}
	}

	for (const TSubclassOf<UGameplayAbility>& StartAbility: StartAbilities)
	{
		if (StartAbility)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(StartAbility));
		}
	}
	
	GSGAS_LOG(LogGSGAS, Log, TEXT("Enemy ASC initialized on %s."), *GetName());
}

void AGSGASCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AGSGASCharacterEnemy::OnOutOfHealth()
{
	OnDeath();
}

void AGSGASCharacterEnemy::OnDeath()
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

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetSimulatePhysics(true);
	}

	GSGAS_LOG(LogGSGAS, Log, TEXT("%s died."), *GetName());
}
