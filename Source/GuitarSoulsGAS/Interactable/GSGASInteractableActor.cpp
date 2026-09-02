// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/GSGASInteractableActor.h"

AGSGASInteractableActor::AGSGASInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGSGASInteractableActor::Interact_Implementation(AActor* InteractionActor)
{
	// 의도적으로 비어 있음 — 파생 클래스 또는 BP의 Interact 이벤트가 담당한다.
}

FText AGSGASInteractableActor::GetInteractPromptText_Implementation() const
{
	return InteractPromptText;
}
