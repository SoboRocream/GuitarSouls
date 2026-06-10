// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSGASWeaponCollisionComponent.h"

#include "GuitarSoulsGAS.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"

static TAutoConsoleVariable<int32> CVarDrawGSGASWeaponCollision(
	TEXT("gsgas.debug.weaponcollision"),
	0,
	TEXT("Draws debug shapes for GSGAS weapon collision. 0: Off, 1: On"),
	ECVF_Cheat);

// Sets default values for this component's properties
UGSGASWeaponCollisionComponent::UGSGASWeaponCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
 
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

void UGSGASWeaponCollisionComponent::TurnOnCollision()
{
	if (!WeaponMesh)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponMesh is null. Call SetWeaponMesh() before TurnOnCollision."));
		return;
	}
 
	AlreadyHitActors.Empty();
	bCollisionEnabled = true;
	SetComponentTickEnabled(true);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("Collision ON — Start: %s / End: %s"),
		*TraceStartSocketName.ToString(), *TraceEndSocketName.ToString());
}

void UGSGASWeaponCollisionComponent::TurnOffCollision()
{
	bCollisionEnabled = false;
	SetComponentTickEnabled(false);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("Collision OFF"));
}

void UGSGASWeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* InMesh)
{
	if (!InMesh)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("InMesh is null."));
		return;
	}
 
	WeaponMesh = InMesh;
}

void UGSGASWeaponCollisionComponent::AddIgnoreActor(AActor* InActor)
{
	if (InActor)
	{
		IgnoreActors.AddUnique(InActor);
	}
}

void UGSGASWeaponCollisionComponent::RemoveIgnoreActor(AActor* InActor)
{
	IgnoreActors.Remove(InActor);
}

void UGSGASWeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCollisionEnabled)
	{
		PerformCollisionTrace();
	}
}

bool UGSGASWeaponCollisionComponent::CanHitActor(const AActor* InActor) const
{
	return !AlreadyHitActors.Contains(InActor);
}

void UGSGASWeaponCollisionComponent::PerformCollisionTrace()
{
	if (!WeaponMesh)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponMesh is null during trace. Turning off collision."));
		TurnOffCollision();
		return;
	}
 
	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End   = WeaponMesh->GetSocketLocation(TraceEndSocketName);
 
	// TObjectPtr → TArray<AActor*> 변환 (KismetSystemLibrary API 요구사항)
	TArray<AActor*> RawIgnoreActors;
	RawIgnoreActors.Reserve(IgnoreActors.Num());
	for (const TObjectPtr<AActor>& Actor : IgnoreActors)
	{
		if (Actor)
		{
			RawIgnoreActors.Add(Actor);
		}
	}
 
	const bool bDrawDebug = CVarDrawGSGASWeaponCollision.GetValueOnGameThread() > 0;
	const EDrawDebugTrace::Type DebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
 
	TArray<FHitResult> OutHitResults;
	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TraceRadius,
		TraceObjectTypes,
		false,
		RawIgnoreActors,
		DebugType,
		OutHitResults,
		true);
 
	if (!bHit)
	{
		return;
	}
 
	for (const FHitResult& HitResult : OutHitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor || !CanHitActor(HitActor))
		{
			continue;
		}
 
		AlreadyHitActors.Add(HitActor);

		GSGAS_LOG(LogGSGAS, Log, TEXT("Hit: %s"), *HitActor->GetName());

		OnHitActor.Broadcast(HitResult);

		if (HitParticle)
		{
			bool bBlocked = false;
			if (!ParticleBlockedTags.IsEmpty())
			{
				if (UAbilitySystemComponent* TargetASC =
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					bBlocked = TargetASC->HasAnyMatchingGameplayTags(ParticleBlockedTags);
				}
			}

			if (!bBlocked)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), HitParticle,
					HitResult.ImpactPoint,
					HitResult.ImpactNormal.Rotation());
			}
		}
	}
}

