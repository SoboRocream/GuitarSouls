// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSGASTargetingComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GuitarSoulsGAS.h"
#include "Data/GSGASCollision.h"
#include "GameFramework/Character.h"
#include "Interface/GSGASTargetingInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Tags/GSGASGameplayTags.h"

// Sets default values for this component's properties
UGSGASTargetingComponent::UGSGASTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void UGSGASTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Camera = Character->GetComponentByClass<UCameraComponent>();
	}
}


// Called every frame
void UGSGASTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsLockOn) return;
	if (!IsValid(Character)) return;
	if (!IsValid(LockedTargetActor))
	{
		StopLockOn();
		return;
	}

	const float Distance = FVector::Distance(
		Character->GetActorLocation(), LockedTargetActor->GetActorLocation());

	if (!CanBeTargeted(LockedTargetActor) || Distance > TargetingRadius)
	{
		StopLockOn();
		return;
	}

	FaceLockOnActor();
}

void UGSGASTargetingComponent::LockOn()
{
	TArray<AActor*> OutTargets;
	FindTargets(OutTargets);

	AActor* TargetActor = FindClosestTarget(OutTargets);
	if (!IsValid(TargetActor))
	{
		GSGAS_LOG(LogGSGAS, Log, TEXT("No valid target found."));
		return;
	}

	LockedTargetActor = TargetActor;
	bIsLockOn = true;
	SetComponentTickEnabled(true);

	if (IGSGASTargetingInterface* Targeting = Cast<IGSGASTargetingInterface>(LockedTargetActor))
	{
		Targeting->OnTargeted(true);
	}

	OrientCamera();

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_LockOn);
		}
	}
	
	GSGAS_LOG(LogGSGAS, Log, TEXT("LockOn: %s"), *LockedTargetActor->GetName());

}

void UGSGASTargetingComponent::StopLockOn()
{
	if (IGSGASTargetingInterface* Targeting = Cast<IGSGASTargetingInterface>(LockedTargetActor))
	{
		Targeting->OnTargeted(false);
	}

	LockedTargetActor = nullptr;
	bIsLockOn = false;
	SetComponentTickEnabled(false);

	OrientMovement();

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_LockOn);
		}
	}
	GSGAS_LOG(LogGSGAS, Log, TEXT("LockOn stopped."));
}

void UGSGASTargetingComponent::FindTargets(TArray<AActor*>& OutTargetActors) const
{
	TArray<FHitResult> OutHits;
	const FVector Origin = Character->GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CCHANNEL_GSGAS_TARGETING));

	TArray<AActor*> IgnoreActors;
	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Origin,
		Origin,
		TargetingRadius,
		ObjectTypes,
		false,
		IgnoreActors,
		DrawDebugType,
		OutHits,
		true);

	if (!bHit) return;

	for (const FHitResult& HitResult : OutHits)
	{
		AActor* HitActor = HitResult.GetActor();
		if (CanBeTargeted(HitActor))
		{
			OutTargetActors.Add(HitActor);
		}
	}
}

AActor* UGSGASTargetingComponent::FindClosestTarget(TArray<AActor*>& InTargets) const
{
	if (!Camera) return nullptr;

	float BestDot = -1.f;
	AActor* BestTarget = nullptr;

	for (AActor* TargetActor : InTargets)
	{
		if (!IsValid(TargetActor)) continue;
		if (LockedTargetActor == TargetActor) continue;

		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
			Character->GetActorLocation(), TargetActor->GetActorLocation());
		const float Dot = FVector::DotProduct(Camera->GetForwardVector(), LookAt.Vector());
		if (Dot > BestDot)
		{
			BestDot = Dot;
			BestTarget = TargetActor;
		}

	}

	return BestTarget;
	
}

bool UGSGASTargetingComponent::CanBeTargeted(AActor* InActor) const
{
	if (!IsValid(InActor)) return false;
	UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor);
	if (!TargetASC) return false;
	
	return !TargetASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_Death);
}

void UGSGASTargetingComponent::OrientCamera() const
{
	if (!Character) return;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void UGSGASTargetingComponent::OrientMovement() const
{
	if (!Character) return;
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void UGSGASTargetingComponent::FaceLockOnActor() const
{
	if (!Character || !Camera || !LockedTargetActor) return;
	AController* Controller = Character->GetController();
	if (!Controller) return;

	const FRotator CurrentControlRotation = Character->GetControlRotation();
	const FVector TargetLocation = LockedTargetActor->GetActorLocation() - FVector(0.f, 0.f, 150.f);
	const FRotator TargetLookAt = UKismetMathLibrary::FindLookAtRotation(
		Character->GetActorLocation(), TargetLocation);

	const FRotator InterpRotation = FMath::RInterpTo(CurrentControlRotation, TargetLookAt, GetWorld()->GetDeltaSeconds(), FaceLockOnRotationSpeed);

	Controller->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw, CurrentControlRotation.Roll));
}

