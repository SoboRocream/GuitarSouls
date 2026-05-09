// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSGASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GuitarSoulsGAS.h"
#include "Player/GSGASPlayerState.h"
#include "Abilities/GameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Data/GSGASCollision.h"
#include "Interface/GSGASInteractInterface.h"
#include "Tags/GSGASGameplayTags.h"
#include "UI/GSGASPlayerHUDWidget.h"

AGSGASCharacterPlayer::AGSGASCharacterPlayer()
{
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	InteractDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractDetectionSphere"));
	InteractDetectionSphere->SetupAttachment(RootComponent);
	InteractDetectionSphere->SetSphereRadius(InteractRadius);
	InteractDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractDetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractDetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractDetectionSphere->SetCollisionResponseToChannel(
		CCHANNEL_GSGAS_INTERACTION, ECR_Overlap);
}

void AGSGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AGSGASPlayerState* GASPS = GetPlayerState<AGSGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		// 공통 어빌리티 부여 (Base에서 정의)
		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}

		// 입력 어빌리티 부여 (Player 전용)
		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
			StartSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		SetupGASInputComponent();

			// 초기 GE 적용 — CurrentValue를 채워줌으로써 HUD 초기값 정상 표시 및 기타 초기 effect 적용
		for (const auto& InitEffect : InitEffects)
		{
			if (InitEffect)
			{
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitEffect, 1.f, ContextHandle);
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}

		// APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		// PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void AGSGASCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// HUD 생성 및 ASC 연결
	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UGSGASPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();

			// PlayerState에서 ASC 획득 후 위젯에 주입
			if (AGSGASPlayerState* GASPS = GetPlayerState<AGSGASPlayerState>())
			{
				PlayerHUDWidget->SetAbilitySystemComponent(GASPS);
			}
		}
	}

	InteractDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGSGASCharacterPlayer::OnInteractSphereBeginOverlap);
	InteractDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AGSGASCharacterPlayer::OnInteractSphereEndOverlap);
}

void AGSGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGSGASCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGSGASCharacterPlayer::Look);

	SetupGASInputComponent();
}

void AGSGASCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		// 약공 (InputID: 1)
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AGSGASCharacterPlayer::GASInputPressed, 1);

		// 강공 (InputID: 2) — Shift 조합
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AGSGASCharacterPlayer::GASInputPressed, 2);

		// 상호작용 (InputID: 3)
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 3);

		// 회피 (InputID: 4)
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 4);

		// 락온 (InputID: 5
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 5);
	}
}

void AGSGASCharacterPlayer::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void AGSGASCharacterPlayer::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void AGSGASCharacterPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D CurrentMovementInput = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, LastMovementInput.X);
		AddMovementInput(RightDirection, LastMovementInput.Y);

		LastMovementInput = CurrentMovementInput;
	}
}

void AGSGASCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGSGASCharacterPlayer::OnInteractSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (Cast<IGSGASInteractInterface>(OtherActor))
	{
		if (PlayerHUDWidget)
		{
			//PlayerHUDWidget->ShowInteractUI(FText::FromString(TEXT("[E] 상호작용")));
		}
		GSGAS_LOG(LogGSGAS, Log, TEXT("Interact target entered: %s"), *OtherActor->GetName());
	}
}

void AGSGASCharacterPlayer::OnInteractSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	if (Cast<IGSGASInteractInterface>(OtherActor))
	{
		if (PlayerHUDWidget)
		{
			//PlayerHUDWidget->HideInteractUI();
		}
		GSGAS_LOG(LogGSGAS, Log, TEXT("Interact target exited: %s"), *OtherActor->GetName());
	}
}

void AGSGASCharacterPlayer::OnOutOfHealth()
{
	OnDeath();
}

void AGSGASCharacterPlayer::OnDeath()
{
	// 입력 차단
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}
 
	// 콜리전 비활성화
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
 
	// 래그돌
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComponent->SetSimulatePhysics(true);
	}
 
	// 사망 태그 부여
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_Death);
	}
}
