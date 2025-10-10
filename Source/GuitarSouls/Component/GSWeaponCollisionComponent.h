// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GSWeaponCollisionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitActor, const FHitResult&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUITARSOULS_API UGSWeaponCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnHitActor OnHitActor;

protected:
	UPROPERTY(EditAnywhere)
	FName TraceStartSocketName;
	
	UPROPERTY(EditAnywhere)
	FName TraceEndSocketName;

protected:
	// Sphere
	UPROPERTY(EditAnywhere)
	float TraceRadius = 20.f;

	// Trace Targets
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	// Ignore Actors
	UPROPERTY(EditAnywhere)
	TArray<AActor*> IgnoreActors;

	// // Draw Debug Type
	// UPROPERTY(EditAnywhere)
	// TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	// Weapon's MeshComponent(StaticMesh, SkeletalMesh, etc.)
	UPROPERTY()
	UPrimitiveComponent* WeaponMesh;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	bool bISCollisionEnabled = false;

public:
	UGSWeaponCollisionComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TurnOnCollision();

	void TurnOffCollision();

	void SetMeshWeapon(UPrimitiveComponent* Mesh);

	void AddIgnoreActor(AActor* InActor);

	void RemoveIgnoreActor(AActor* Actor);

protected:
	bool CanHitActor(AActor* Actor) const;

	void CollisionTrace();
};
