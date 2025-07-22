// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


AGSCharacterPlayer::AGSCharacterPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
}

void AGSCharacterPlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	// if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	// {
	// 	Subsystem->ClearAllMappings();
	// 	UInputMappingContext* NewMappingContext = ;
	// 	if (NewMappingContext)
	// 	{
	// 		Subsystem->AddMappingContext(NewMappingContext, 0);
	// 	}
	// }
}
