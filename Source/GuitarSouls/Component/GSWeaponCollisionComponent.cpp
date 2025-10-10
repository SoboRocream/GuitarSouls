// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSWeaponCollisionComponent.h"

UGSWeaponCollisionComponent::UGSWeaponCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType((ECC_Pawn)));
}

void UGSWeaponCollisionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bISCollisionEnabled)
	{
		CollisionTrace();
	}
}

void UGSWeaponCollisionComponent::TurnOnCollision()
{
	AlreadyHitActors.Empty();
	bISCollisionEnabled = true;
}

void UGSWeaponCollisionComponent::TurnOffCollision()
{
	bISCollisionEnabled = false;
}

void UGSWeaponCollisionComponent::SetMeshWeapon(UPrimitiveComponent* Mesh)
{
	WeaponMesh = Mesh;
}

void UGSWeaponCollisionComponent::AddIgnoreActor(AActor* InActor)
{
	IgnoreActors.Add(InActor);
}

void UGSWeaponCollisionComponent::RemoveIgnoreActor(AActor* Actor)
{
	IgnoreActors.Remove(Actor);
}

bool UGSWeaponCollisionComponent::CanHitActor(AActor* Actor) const
{
	return AlreadyHitActors.Contains(Actor) == false;
}

static TAutoConsoleVariable<int32> CVarDrawWeaponCollision(
	TEXT("gs.debug.weaponcollision"),
	0, 
	TEXT("Draws debug spheres for weapon collision. 0: Off, 1: On"),
	ECVF_Cheat);

void UGSWeaponCollisionComponent::CollisionTrace()
{
	TArray<FHitResult> OutHitResults;

	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

	const bool bDrawDebug = CVarDrawWeaponCollision.GetValueOnGameThread() > 0;
	const EDrawDebugTrace::Type DebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TraceRadius,
		TraceObjectTypes,
		false,
		IgnoreActors,
		DebugType,
		OutHitResults,
		true);

	if (bHit)
	{
		for (const FHitResult& HitResult: OutHitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor == nullptr)
			{
				continue;
			}

			if (CanHitActor(HitActor))
			{
				AlreadyHitActors.Add(HitActor);

				if (OnHitActor.IsBound())
				{
					OnHitActor.Broadcast(HitResult);
				}
			}
		}
			
	}
// #if ENABLE_DRAW_DEBUG
//
// 	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
// 	float CapsuleHalfHeight = FVector::Distance(Start, End) * 0.5f;
// 	FColor DrawColor = bHit ? FColor::Green : FColor::Orange;
//
// 	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, TraceRadius, FRotationMatrix::MakeFromZ((End - Start).GetSafeNormal()).ToQuat(), DrawColor, false, 1.0f);
//
// #endif
}
