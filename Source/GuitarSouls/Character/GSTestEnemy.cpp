// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSTestEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "GSCharacterStateComponent.h"
#include "AI/GSEnemyAIController.h"
#include "Component/GSAttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "GSEnumDefine.h"
#include "Component/GSCombatComponent.h"
#include "Component/GSRotationComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"
#include "Physics/GSCollision.h"
#include "Tag/GSGameplayTags.h"
#include "UI/GSStatBarWidget.h"

AGSTestEnemy::AGSTestEnemy()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	TargetingSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphereComponent->SetupAttachment(GetRootComponent());
	TargetingSphereComponent->SetCollisionObjectType(CCHANNEL_GSTARGETING);
	TargetingSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidgetComponent->SetupAttachment(GetRootComponent());
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UGSAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UGSCharacterStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UGSCombatComponent>(TEXT("Combat"));
	RotationComponent = CreateDefaultSubobject<UGSRotationComponent>(TEXT("Rotation"));
	
	HpBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBar"));
	HpBarWidgetComponent->SetupAttachment(GetRootComponent());
	HpBarWidgetComponent->SetRelativeLocation(FVector(0.f,0.f,100.f));
	HpBarWidgetComponent->SetDrawSize(FVector2D(100.f, 5.f));
	HpBarWidgetComponent->SetWidgetSpace((EWidgetSpace::Screen));
	HpBarWidgetComponent->SetVisibility(false);
	
	AttributeComponent->OnDeath.AddUObject(this, &AGSTestEnemy::OnDeath);
	AttributeComponent->OnAttributeChanged.AddUObject(this, &AGSTestEnemy::OnAttributeChanged);
	
}

void AGSTestEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		AGSWeapon* Weapon = GetWorld()->SpawnActor<AGSWeapon>(DefaultWeaponClass, GetActorTransform(), Params);
		CombatComponent->SetCombatEnabled(true);
		Weapon->EquipItem();
	}

	SetupHpBar();
}

float AGSTestEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("@@****DAMAGE APPLIED****@@"));
	
	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
		GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Damage: %f"), ActualDamage));
	}
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		FVector ShotDirection = PointDamageEvent.ShotDirection;
		FVector ImpactPoint = PointDamageEvent.HitInfo.ImpactPoint;
		FVector ImpactDirection = PointDamageEvent.HitInfo.ImpactNormal;
		FVector HitLocation = PointDamageEvent.HitInfo.Location;

		UAISense_Damage::ReportDamageEvent(GetWorld(),this, EventInstigator->GetPawn(), ActualDamage, HitLocation, HitLocation);
		
		ImpactEffect(ImpactPoint);
		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
	
}

void AGSTestEnemy::OnDeath()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
	
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetCollisionProfileName("Ragdoll");
		MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComponent->SetSimulatePhysics(true);
	}

	if (AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon())
	{
		MainWeapon->Drop();
	}

	SetDeathState();
}

void AGSTestEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ParriedDelayTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StunnedDelayTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AGSTestEnemy::SetDeathState()
{
	if (StateComponent)
	{
		StateComponent->SetState(GSGameplayTags::Character_State_Death);
	}

	if (AGSEnemyAIController* AIController = Cast<AGSEnemyAIController>(GetController()))
	{
		AIController->StopUpdateTarget();
	}
	
	ToggleHpBarVisibility(false);
}

void AGSTestEnemy::OnAttributeChanged(EGSEnumDefine AttributeType, float InValue)
{
	if (AttributeType == EGSEnumDefine::Health)
	{
		if (HpBarWidgetComponent)
		{
			if (const UGSStatBarWidget* StatBar = Cast<UGSStatBarWidget>(HpBarWidgetComponent->GetWidget()))
			{
				StatBar->SetRatio(InValue);
			}
		}
	}
}

void AGSTestEnemy::SetupHpBar()
{
	if (HpBarWidgetComponent)
	{
		if (UGSStatBarWidget* StatBar = Cast<UGSStatBarWidget>(HpBarWidgetComponent->GetWidget()))
		{
			StatBar->FillColorAndOpacity = FLinearColor::Red;
		}
	}

	if (AttributeComponent)
	{
		AttributeComponent->BroadCastAttributeChanged(EGSEnumDefine::Health);
	}
}

void AGSTestEnemy::ImpactEffect(const FVector& Location)
{
	if (ImpactSound)
	{
		UE_LOG(LogTemp, Log, TEXT("@@****ImpactSound APPLIED****@@"));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle)
	{
		UE_LOG(LogTemp, Log, TEXT("@@****ImpactParticle APPLIED****@@"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void AGSTestEnemy::HitReaction(const AActor* Attacker)
{
	check(CombatComponent)
	check(StateComponent)
	
	if (StunnedRate >= FMath::RandRange(1, 100))
	{
		StateComponent->SetState(GSGameplayTags::Character_State_Stunned);
		// StunnedDelay = FMath::RandRange(0.5f, 3.f);
	}
	
	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
	{
		const float DelaySeconds = PlayAnimMontage(HitReactAnimMontage) + StunnedDelay;

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			FGameplayTagContainer CheckTags;
			CheckTags.AddTag(GSGameplayTags::Character_State_Stunned);
			if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
			{
				StateComponent->ClearState();
			}
		});
		GetWorld()->GetTimerManager().SetTimer(StunnedDelayTimerHandle, TimerDelegate, DelaySeconds, false);;
	}
}

void AGSTestEnemy::OnTargeted(bool bTargeted)
{
	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
}

bool AGSTestEnemy::CanBeTargeted()
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer TagCheck;
	TagCheck.AddTag(GSGameplayTags::Character_State_Death);
	return StateComponent->IsCurrentStateEqualToAny(TagCheck) == false;
}

void AGSTestEnemy::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void AGSTestEnemy::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}

void AGSTestEnemy::PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate)
{
	check(StateComponent);
	check(AttributeComponent);
	check(CombatComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Stunned);
	if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
	{
		return;
	}

	if (const AGSWeapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(GSGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		AttributeComponent->ToggleStaminaRegen(false);

		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag))
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegen(true, 1.5f);
	}
}

void AGSTestEnemy::Parried()
{
	check(StateComponent);
	check(CombatComponent);

	StopAnimMontage();
	StateComponent->SetState(GSGameplayTags::Character_State_Parried);

	if (const AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon())
	{
		UAnimMontage* ParriedMontage = MainWeapon->GetMontageForTag(GSGameplayTags::Character_State_Parried);
		const float Delay = PlayAnimMontage(ParriedMontage) + 1.f;

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			FGameplayTagContainer CheckTags;
			CheckTags.AddTag(GSGameplayTags::Character_State_Death);
			if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false)
			{
				StateComponent->ClearState();
			}
		});
		GetWorld()->GetTimerManager().SetTimer(ParriedDelayTimerHandle, TimerDelegate, Delay, false);
	}
}

void AGSTestEnemy::ToggleHpBarVisibility(bool bVisibility)
{
	if (HpBarWidgetComponent)
	{
		HpBarWidgetComponent->SetVisibility(bVisibility);
	}
}

void AGSTestEnemy::WatchTarget(AActor* InTargetActor)
{
	
}
