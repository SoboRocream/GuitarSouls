// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterBase.h"

// Sets default values
AGSCharacterBase::AGSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

