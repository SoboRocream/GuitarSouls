// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GSCharacterControlData.h"
#include "GSCharacterStateComponent.h"
#include "Component/GSAttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/GSPlayerHUDWidget.h"
#include "Component/GSCombatComponent.h"
#include "Component/GSTargetingComponent.h"
#include "Interface/GSInteractInterface.h"
#include "Item/GSFistWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tag/GSGameplayTags.h"
#include "Physics/GSCollision.h"
#include "DrawDebugHelpers.h"
#include "Animation/GSAnimInstance.h"
#include "Component/GSPotionInventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

AGSCharacterPlayer::AGSCharacterPlayer()
{
	// Camera Section
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	// Movement Section
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh Section
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Input Section
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRef(TEXT("/Game/GuitarSouls/Input/Action/IA_Move.IA_Move"));
	if (nullptr != IA_MoveRef.Object)
	{
		MoveAction = IA_MoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookRef(TEXT("/Game/GuitarSouls/Input/Action/IA_Look.IA_Look"));
	if (nullptr != IA_LookRef.Object)
	{
		LookAction = IA_LookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SprintRef(TEXT("/Game/GuitarSouls/Input/Action/IA_Sprint_Roll.IA_Sprint_Roll"));
	if (nullptr != IA_SprintRef.Object)
	{
		SprintRollingAction = IA_SprintRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_InteractRef(TEXT("/Game/GuitarSouls/Input/Action/IA_Interact.IA_Interact"));
	if (nullptr != IA_SprintRef.Object)
	{
		InteractAction = IA_InteractRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ToggleCombatRef(TEXT("/Game/GuitarSouls/Input/Action/IA_ToggleCombat.IA_ToggleCombat"));
	if (nullptr != IA_ToggleCombatRef.Object)
	{
		ToggleCombatAction = IA_ToggleCombatRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackRef(TEXT("/Game/GuitarSouls/Input/Action/IA_Attack.IA_Attack"));
	if (nullptr != IA_AttackRef.Object)
	{
		AttackAction = IA_AttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_HeavyAttackRef(TEXT("/Game/GuitarSouls/Input/Action/IA_HeavyAttack.IA_HeavyAttack"));
	if (nullptr != IA_HeavyAttackRef.Object)
	{
		HeavyAttackAction = IA_HeavyAttackRef.Object;
	}
	
	CurrentCharacterControlType = ECharacterControlType::Default;

	// Component Section
	AttributeComponent = CreateDefaultSubobject<UGSAttributeComponent>(TEXT("GSAttribute"));
	StateComponent = CreateDefaultSubobject<UGSCharacterStateComponent>(TEXT("GSCharacterState"));
	CombatComponent = CreateDefaultSubobject<UGSCombatComponent>(TEXT("CombatComponent"));
	TargetingComponent = CreateDefaultSubobject<UGSTargetingComponent>(TEXT("TargetingComponent"));
	PotionInventoryComponent = CreateDefaultSubobject<UGSPotionInventoryComponent>(TEXT("PotionInventoryComponent"));

	// Animation Section
	static ConstructorHelpers::FObjectFinder<class UAnimMontage> RollingMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/_Assets/Animations/Sword_and_Shield/Roll/AM_Roll_Forward.AM_Roll_Forward'"));
	if (RollingMontageRef.Succeeded())
	{
		RollingMontage = RollingMontageRef.Object;
	}

	// OnDeath Delegate
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
}

void AGSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGSCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGSCharacterPlayer::Look);

	EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &AGSCharacterPlayer::Sprint);
	EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &AGSCharacterPlayer::StopSprint);
	EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &AGSCharacterPlayer::Rolling);

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::Interact);
	EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::ToggleCombat);
	
	// EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AGSCharacterPlayer::ChangeCharacterControl);

	// Attack
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::AutoToggleCombat);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &AGSCharacterPlayer::Attack);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGSCharacterPlayer::SpecialAttack);
	EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this,  &AGSCharacterPlayer::HeavyAttack);

	// Lock On
	EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this,  &AGSCharacterPlayer::LockOnTarget);
	EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this,  &AGSCharacterPlayer::LeftTarget);
	EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::RightTarget);

	// Blocking
	EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::Blocking);
	EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AGSCharacterPlayer::BlockingEnd);

	// Parrying
	EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::Parrying);

	// Drink
	EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &AGSCharacterPlayer::UsePotion);
}

void AGSCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
	#if WITH_EDITOR
	PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	#endif
}

bool AGSCharacterPlayer::IsDeath() const
{
	check(StateComponent);
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Death);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags);
}

float AGSCharacterPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                     class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!CanReceiveDamage())
	{
		UE_LOG(LogTemp, Warning, TEXT("Rolling IFrames"));
		return ActualDamage;
	}
	check(AttributeComponent);
	check(StateComponent);

	InterruptWhileDrinking();

	bFacingEnemy = UKismetMathLibrary::InRange_FloatFloat(GetDotProductTo(EventInstigator->GetPawn()), -0.1f, 1.f);

	if (ParriedAttackSucceed())
	{
		if (IGSCombatInterface* CombatInterface = Cast<IGSCombatInterface>(EventInstigator->GetPawn()))
		{
			CombatInterface->Parried();

			AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon();
			if (IsValid(MainWeapon))
			{
				FVector Location = MainWeapon->GetActorLocation();
				ShieldBlockEffect(Location);
			}
		}
		return ActualDamage;
	}
	
	if (CanPerformBlocking())
	{
		AttributeComponent->TakeDamageAmount(0.f);
		AttributeComponent->DecreaseStamina(20.f);
		StateComponent->SetState(GSGameplayTags::Character_State_Blocking);
	}
	else
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
		StateComponent->SetState(GSGameplayTags::Character_State_Hit);
	}
	
	StateComponent->ToggleMovementInput(false);;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		FVector ShotDirection = PointDamageEvent->ShotDirection;
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn(), EGSDamageType::Normal);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		const FVector HitLocation = RadialDamageEvent->Origin;

		ImpactEffect(HitLocation);

		HitReaction(EventInstigator->GetPawn(), EGSDamageType::KnockBack);
	}

	return ActualDamage;
}

void AGSCharacterPlayer::ImpactEffect(const FVector& Location)
{
	if (CanPerformBlocking())
	{
		ShieldBlockEffect(Location);
	}
	else
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
		}

		if (ImpactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
		}
	}
	
}

void AGSCharacterPlayer::ShieldBlockEffect(const FVector& Location)
{
	if (BlockSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlockSound, Location);
	}

	if (BlockParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockParticle, Location);
	}
}

void AGSCharacterPlayer::HitReaction(const AActor* Attacker, const EGSDamageType InDamageType)
{
	check(CombatComponent)

	if (CanPerformBlocking())
	{
		if (UAnimMontage* BlockingMontage = CombatComponent->GetMainWeapon()->GetMontageForTag(GSGameplayTags::Character_Action_BlockingHit))
		{
			PlayAnimMontage(BlockingMontage);
		}
	}
	else
	{
		if (InDamageType == EGSDamageType::Normal)
		{
			if (UAnimMontage* HitReactionAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
			{
				PlayAnimMontage(HitReactionAnimMontage);
			}
		}
		else if (InDamageType == EGSDamageType::KnockBack)
		{
			if (UAnimMontage* HitReactionAnimMontage = CombatComponent->GetMainWeapon()->GetMontageForTag(GSGameplayTags::Character_Action_KnockBackHit))
			{
				PlayAnimMontage(HitReactionAnimMontage);
			}
		}
		
	}
}

void AGSCharacterPlayer::OnDeath()
{
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
}

void AGSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterControlType);

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UGSPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (FistWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AGSFistWeapon* FistWeapon = GetWorld()->SpawnActor<AGSFistWeapon>(FistWeaponClass, GetActorTransform(), SpawnParams);
		FistWeapon->EquipItem();
	}
}

void AGSCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const UWorld* World = GetWorld();
	if (World)
	{
		// 화살표의 시작점 (캐릭터의 현재 위치)
		const FVector StartPoint = GetActorLocation();

		// 캐릭터의 순수한 앞 방향 (크기가 1인 정규화된 벡터)
		const FVector ForwardVector = GetActorForwardVector();

		// 화살표 길이 (100.0f = 1미터) - 눈에 잘 띄도록 길이를 늘려줍니다.
		const float ArrowLength = 150.0f;

		// 화살표의 끝점 계산 (시작점 + 방향 * 길이)
		const FVector EndPoint = StartPoint + (ForwardVector * ArrowLength);

		// 디버그 화살표를 그립니다.
		DrawDebugLine(
			World,
			StartPoint,
			EndPoint,
			FColor::Red,    // 색상 (보통 X축, 즉 Forward는 빨간색으로 표현)
			false,          // 지속성 여부 (false면 1프레임만 표시)
			0.f,            // 지속 시간 (지속성이 false면 의미 없음)
			0,              // 우선순위 (기본값)
			2.0f            // 두께
		);
	}
	
	//GEngine->AddOnScreenDebugMessage(0,1.5f, FColor::Cyan, FString::Printf(TEXT("Stamina: %f"), AttributeComponent->GetBaseStamina()));
	//GEngine->AddOnScreenDebugMessage(2,1.5f, FColor::Cyan, FString::Printf(TEXT("MaxWalkSpeed: %f"), GetCharacterMovement()->MaxWalkSpeed));

}

void AGSCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Default)
	{
		SetCharacterControl(ECharacterControlType::Default);
	}
}

void AGSCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UGSCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			if (UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext)
			{
				Subsystem->AddMappingContext(NewMappingContext, 0);
			}
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AGSCharacterPlayer::SetCharacterControlData(const class UGSCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	
}

void AGSCharacterPlayer::Move(const FInputActionValue& Value)
{
	check(StateComponent);
	if (StateComponent->MovementInputEnabled() == false)
	{
		return;
	}
	
	LastMovementInput = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, LastMovementInput.X);
		AddMovementInput(RightDirection, LastMovementInput.Y);
	}

}

void AGSCharacterPlayer::Look(const FInputActionValue& Value)
{
	// Lock on 시 입력차단, double check => prevent Crash
	if (TargetingComponent && TargetingComponent->IsLockedOn())
	{
		return;
	}
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

bool AGSCharacterPlayer::IsMoving() const
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		return Movement->Velocity.Size2D() > 3.0f && Movement->GetCurrentAcceleration() != FVector::ZeroVector;
	}
	return false;
}

bool AGSCharacterPlayer::CanToggleCombat() const
{
	check(StateComponent);

	if (IsValid(CombatComponent->GetMainWeapon()) == false)
	{
		return false;
	}

	if (CombatComponent->GetMainWeapon()->GetCombatType() == ECombatType::MeleeFists)
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(GSGameplayTags::Character_State_GeneralAction);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false;
}

void AGSCharacterPlayer::Sprint()
{
	check(CombatComponent);
	check(AttributeComponent);
	
	if (CombatComponent->IsBlockingEnabled())
	{
		return;
	}
	// 가만히 있는 경우에 대한 조건 추가해야함
	if (AttributeComponent->CheckHasEnoughStamina(5.f) && IsMoving())
	{
		AttributeComponent->ToggleStaminaRegen(false);
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		AttributeComponent->DecreaseStamina(0.2f);

		bSprinting = true;
	}
	else
	{
		StopSprint();
	}
}

void AGSCharacterPlayer::StopSprint()
{
	check(CombatComponent);
	check(AttributeComponent);
	
	if (CombatComponent->IsBlockingEnabled())
	{
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComponent->ToggleStaminaRegen(true, 2.f);
	
	bSprinting = false;
}

void AGSCharacterPlayer::Rolling()
{
	check(AttributeComponent);
	check(StateComponent);
	
	if (StateComponent->GetState() != GSGameplayTags::Character_State_Rolling)
	{
		const UCharacterMovementComponent* Movement = CastChecked<UCharacterMovementComponent>(GetCharacterMovement()); 
		if (AttributeComponent->CheckHasEnoughStamina(15.f))
		{
			AttributeComponent->ToggleStaminaRegen(false);
			AttributeComponent->DecreaseStamina(15.f);
			
			const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
				
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			FVector InputDirection = (ForwardDirection * LastMovementInput.X) + (RightDirection * LastMovementInput.Y);
			FVector RollingDirection = InputDirection.GetSafeNormal();
				
			if (!RollingDirection.IsNearlyZero())
			{
				SetActorRotation(RollingDirection.Rotation());
			}
			
			PlayAnimMontage(RollingMontage);
			
			StateComponent->ToggleMovementInput(false);
			StateComponent->SetState(GSGameplayTags::Character_State_Rolling);
		
			AttributeComponent->ToggleStaminaRegen(true, 1.5f);
		}
	}
	
}

static TAutoConsoleVariable<int32> CVarDrawInteractCollision(
	TEXT("gs.debug.interactcollision"),
	0, 
	TEXT("Draws debug spheres for interact collision. 0: Off, 1: On"),
	ECVF_Cheat);

void AGSCharacterPlayer::Interact()
{
	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 100.f;

	const bool bDrawDebug = CVarDrawInteractCollision.GetValueOnGameThread() > 0;
	const EDrawDebugTrace::Type DebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CCHANNEL_GSINTERACTION));

	TArray<AActor*> ActorsToIgnore;
	
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DebugType,
		OutHit,
		true);

	if (bIsHit)
	{
		if (AActor* HitActor = OutHit.GetActor())
		{
			if (IGSInteractInterface* InteractInterface = Cast<IGSInteractInterface>(HitActor))
			{
				InteractInterface->Interact(this);
			}
		}
	}
}

void AGSCharacterPlayer::ToggleCombat()
{
	check(CombatComponent);
	check(StateComponent);

	if (CombatComponent)
	{
		if (const AGSWeapon* Weapon = CombatComponent->GetMainWeapon())
		{
			if (CanToggleCombat())
			{
				StateComponent->SetState(GSGameplayTags::Character_State_GeneralAction);
				const FGameplayTag MontageTag = CombatComponent->IsCombatEnabled()
					? GSGameplayTags::Character_Action_Unequip
					: GSGameplayTags::Character_Action_Equip;
				PlayAnimMontage(Weapon->GetMontageForTag(MontageTag));
			}
		}
	}
}

void AGSCharacterPlayer::AutoToggleCombat()
{
	if (CombatComponent)
	{
		if (!CombatComponent->IsCombatEnabled())
			ToggleCombat();
	}
}

void AGSCharacterPlayer::Attack()
{
	const FGameplayTag AttackTypeTag = GetAttackPerform();
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void AGSCharacterPlayer::HeavyAttack()
{
	const FGameplayTag AttackTypeTag = GSGameplayTags::Character_Attack_Heavy;
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void AGSCharacterPlayer::SpecialAttack()
{
	const FGameplayTag AttackTypeTag = GSGameplayTags::Character_Attack_Special;
	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

FGameplayTag AGSCharacterPlayer::GetAttackPerform() const
{
	if (IsSprinting())
	{
		return GSGameplayTags::Character_Attack_Running;
	}

	return GSGameplayTags::Character_Attack_Light;
}

bool AGSCharacterPlayer::CanPerformAttack(const FGameplayTag& AttackTypeTag) const
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);

	if (IsValid(CombatComponent->GetMainWeapon()) == false)
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(GSGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(GSGameplayTags::Character_State_Hit);
	CheckTags.AddTag(GSGameplayTags::Character_State_Blocking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Parrying);
	CheckTags.AddTag(GSGameplayTags::Character_State_DrinkPotion);

	const float StaminaCost = CombatComponent->GetMainWeapon()->GetStaminaCost(AttackTypeTag);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false 
		&& CombatComponent->IsCombatEnabled() 
		&& AttributeComponent->CheckHasEnoughStamina(StaminaCost);	
}

void AGSCharacterPlayer::DoAttack(const FGameplayTag& AttackTypeTag)
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);
	if (const AGSWeapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(GSGameplayTags::Character_State_Attacking);
		StateComponent->ToggleMovementInput(false);
		CombatComponent->SetLastAttackType(AttackTypeTag);

		AttributeComponent->ToggleStaminaRegen(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (!Montage)
		{
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}

		PlayAnimMontage(Montage);

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegen(true, 1.5f);
	}
}

void AGSCharacterPlayer::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	if (StateComponent->GetState() != GSGameplayTags::Character_State_Attacking)
	{
		if (bComboSequenceRunning && bCanComboInput == false)
		{
			ComboCounter++;
			UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
			ResetCombo();
			bComboSequenceRunning = true;
		}

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	else if (bCanComboInput == true)
	{
		bSavedComboInput = true;
	}
}

void AGSCharacterPlayer::ResetCombo()
{
	UE_LOG(LogTemp, Warning, TEXT(">>> Combo Reset <<<"));

	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

bool AGSCharacterPlayer::CanPlayerBlockStance() const
{
	check(CombatComponent);
	check(StateComponent);
	check(AttributeComponent);

	if (IsSprinting())
	{
		return false;
	}

	AGSWeapon* Weapon = CombatComponent->GetMainWeapon();
	if (!IsValid(Weapon))
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(GSGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(GSGameplayTags::Character_State_Hit);
	CheckTags.AddTag(GSGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(GSGameplayTags::Character_State_DrinkPotion);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false &&
		Weapon->GetCombatType() == ECombatType::SwordShield &&
			AttributeComponent->CheckHasEnoughStamina(1.f);
}

bool AGSCharacterPlayer::CanPerformBlocking() const
{
	check(CombatComponent);
	check(AttributeComponent);

	return bFacingEnemy && CombatComponent->IsBlockingEnabled() && AttributeComponent->CheckHasEnoughStamina(20.f);
}

bool AGSCharacterPlayer::CanPerformParry() const
{
	check(CombatComponent);
	check(StateComponent);
	check(AttributeComponent);

	AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon();
	if (!IsValid(MainWeapon))
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(GSGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(GSGameplayTags::Character_State_Hit);
	CheckTags.AddTag(GSGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Blocking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Death);
	CheckTags.AddTag(GSGameplayTags::Character_State_Parrying);
	CheckTags.AddTag(GSGameplayTags::Character_State_DrinkPotion);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false &&
		MainWeapon->GetCombatType() == ECombatType::SwordShield &&
			AttributeComponent->CheckHasEnoughStamina(1.f);
}

bool AGSCharacterPlayer::CanDrinkPotion() const
{
	check(PotionInventoryComponent)
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Blocking);
	CheckTags.AddTag(GSGameplayTags::Character_State_Death);
	CheckTags.AddTag(GSGameplayTags::Character_State_GeneralAction);
	CheckTags.AddTag(GSGameplayTags::Character_State_Hit);
	CheckTags.AddTag(GSGameplayTags::Character_State_Parrying);
	CheckTags.AddTag(GSGameplayTags::Character_State_Rolling);

	return PotionInventoryComponent->GetPotionQuantity() > 0 && StateComponent->IsCurrentStateEqualToAny(CheckTags) == false;
}

void AGSCharacterPlayer::InterruptWhileDrinking()
{
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_DrinkPotion);

	if (StateComponent->IsCurrentStateEqualToAny(CheckTags))
	{
		if (PotionInventoryComponent)
		{
			PotionInventoryComponent->DespawnPotion();
		}
	}
}

bool AGSCharacterPlayer::ParriedAttackSucceed() const
{
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(GSGameplayTags::Character_State_Parrying);

	return StateComponent->IsCurrentStateEqualToAny(CheckTags) && bFacingEnemy;
}

void AGSCharacterPlayer::EnableComboWindow()
{
	bCanComboInput = true;
	UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

void AGSCharacterPlayer::DisableComboWindow()
{
	check(CombatComponent);
	bCanComboInput = false;
	if (bSavedComboInput)
	{
		bSavedComboInput = false;
		ComboCounter++;
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next Combo = %d"), ComboCounter);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input Received"));
	}
}

void AGSCharacterPlayer::AttackFinished(const float ComboResetDelay)
{
	UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true);
	}
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &AGSCharacterPlayer::ResetCombo, ComboResetDelay, false);
}

void AGSCharacterPlayer::LockOnTarget()
{
	TargetingComponent->ToggleLockOn();
}

void AGSCharacterPlayer::LeftTarget()
{
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Left);
}

void AGSCharacterPlayer::RightTarget()
{
	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Right);
}

void AGSCharacterPlayer::Blocking()
{
	check(CombatComponent);
	check(StateComponent);

	if (CombatComponent->GetMainWeapon())
	{
		if (CanPlayerBlockStance())
		{
			GetCharacterMovement()->MaxWalkSpeed = BlockingSpeed;
			CombatComponent->SetBlockingEnabled(true);
			if (UGSAnimInstance* AnimInstance = Cast<UGSAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				AnimInstance->UpdateBlocking(true);
				StateComponent->SetState((GSGameplayTags::Character_State_Blocking));
			}
		}
	}
}

void AGSCharacterPlayer::BlockingEnd()
{
	check(CombatComponent);
	check(StateComponent);

	CombatComponent->SetBlockingEnabled(false);
	if (UGSAnimInstance* AnimInstance = Cast<UGSAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->UpdateBlocking(false);
		StateComponent->ClearState();
	}
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void AGSCharacterPlayer::Parrying()
{
	check(StateComponent);
	check(CombatComponent);
	check(AttributeComponent);
	
	if (CanPerformParry())
	{
		if (const AGSWeapon* MainWeapon = CombatComponent->GetMainWeapon())
		{
			UAnimMontage* ParryMontage = MainWeapon->GetMontageForTag(GSGameplayTags::Character_State_Parrying);

			StateComponent->ToggleMovementInput(false);
			AttributeComponent->ToggleStaminaRegen(false);
			AttributeComponent->DecreaseStamina(10.f);

			PlayAnimMontage(ParryMontage);

			AttributeComponent->ToggleStaminaRegen(true, 1.5f);
		}
	}
}

void AGSCharacterPlayer::UsePotion()
{
	if (!StateComponent)
	{
		return;
	}

	if (CanDrinkPotion())
	{
		StateComponent->SetState((GSGameplayTags::Character_State_DrinkPotion));
		PlayAnimMontage(DrinkingMontage);
	}
}

void AGSCharacterPlayer::ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->ActivateCollision(WeaponCollisionType);
	}
}

void AGSCharacterPlayer::DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType)
{
	if (CombatComponent)
	{
		CombatComponent->GetMainWeapon()->DeactivateCollision(WeaponCollisionType);
	}
}

void AGSCharacterPlayer::ToggleIFrames(const bool bEnabled)
{
	bEnabledIFrames = bEnabled;
}