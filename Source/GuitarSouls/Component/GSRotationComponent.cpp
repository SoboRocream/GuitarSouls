// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GSRotationComponent.h"

#include "Kismet/KismetMathLibrary.h"

UGSRotationComponent::UGSRotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGSRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UGSRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TargetActor)
	{
		return;
	}

	if (!bShouldRotate)
	{
		return;
	}

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
	GetOwner()->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
}

