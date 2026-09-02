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
#include "Component/GSGASTargetingComponent.h"
#include "Component/GSGASBerserkComponent.h"
#include "Tags/GSGASGameplayTags.h"
#include "UI/GSGASPlayerHUDWidget.h"
#include "UI/GSGASGameOverWidget.h"
#include "Attribute/GSAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Item/GSGASWeapon.h"
#include "Save/GSGASPlayerSaveData.h"
#include "Save/GSGASPersistenceSubsystem.h"
#include "Engine/GameInstance.h"

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

	// [전시용] 컴포넌트 방식 광폭화 데모 컴포넌트
	BerserkComponent = CreateDefaultSubobject<UGSGASBerserkComponent>(TEXT("BerserkComponent"));
}

void AGSGASCharacterPlayer::ToggleBerserkMode()
{
	bUseComponentBerserk = !bUseComponentBerserk;

	// 컴포넌트 방식 스택 초기화
	if (BerserkComponent)
	{
		BerserkComponent->ResetStacks();
	}

	// GAS 방식 잔여 스택 즉시 제거 (참조가 지정된 경우)
	if (BerserkGASEffectClass)
	{
		if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponent())
		{
			MyASC->RemoveActiveGameplayEffectBySourceEffect(BerserkGASEffectClass, MyASC);
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 3.f,
			bUseComponentBerserk ? FColor::Cyan : FColor::Green,
			FString::Printf(TEXT("Berserk Mode: %s"),
				bUseComponentBerserk ? TEXT("COMPONENT") : TEXT("GAS")));
	}
}

void AGSGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// IMC 등록 + 입력 모드 초기화
	// BeginPlay는 스폰 직후 Controller가 null일 수 있으므로 여기서 처리.
	// UIOnly 상태가 PlayerController에 남아있을 경우(PIE 레벨 재로드 등)를 대비해 GameOnly로 명시 리셋.
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		FInputModeGameOnly GameInputMode;
		PlayerController->SetInputMode(GameInputMode);
		PlayerController->bShowMouseCursor = false;
	}

	AGSGASPlayerState* GASPS = GetPlayerState<AGSGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		if (UGSAttributeSet* AttributeSet = GASPS->GetAttributeSet())
		{
			AttributeSet->OnOutOfHealth.AddUObject(this, &AGSGASCharacterPlayer::OnOutOfHealth);
		}

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

		// 체크포인트 복원 — 저장 데이터가 있으면 InitEffects 초기값을 덮어쓴다.
		// 여기서 Clear하지 않는다. 스냅샷을 남겨야 사망 후 같은 맵을 재로드해도 무기/스탯이 돌아온다.
		// 사망 재시작 경로(MarkDeathRestart)면 리소스를 가득 채워 부활시킨다.
		bool bRestored = false;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGSGASPersistenceSubsystem* Persistence = GI->GetSubsystem<UGSGASPersistenceSubsystem>())
			{
				const bool bFullHeal = Persistence->ConsumeDeathRestart();

				FGSGASPlayerSaveData SaveData;
				if (Persistence->TryGetSaveData(SaveData))
				{
					bRestored = RestoreFromSaveData(SaveData, bFullHeal);
				}
			}
		}

		// 복원할 스냅샷이 없는 경우의 안전망 — 전투 맵에서 맨손으로 시작하는 진행 불가 방지
		if (!bRestored)
		{
			EquipFallbackWeaponIfNeeded();
		}

		// APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		// PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

bool AGSGASCharacterPlayer::CaptureSaveData(FGSGASPlayerSaveData& OutData) const
{
	const AGSGASPlayerState* GASPS = GetPlayerState<AGSGASPlayerState>();
	if (!GASPS)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("CaptureSaveData: PlayerState is null."));
		return false;
	}

	UAbilitySystemComponent* CaptureASC = GASPS->GetAbilitySystemComponent();
	if (!CaptureASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("CaptureSaveData: ASC is null."));
		return false;
	}

	// 소모 리소스: 현재값(모디파이어 반영)을 저장
	OutData.Health = CaptureASC->GetNumericAttribute(UGSAttributeSet::GetHealthAttribute());
	OutData.Stamina = CaptureASC->GetNumericAttribute(UGSAttributeSet::GetStaminaAttribute());
	OutData.PotionCount = CaptureASC->GetNumericAttribute(UGSAttributeSet::GetPotionCountAttribute());

	// 영구 스탯: base값을 저장 — 광폭화 등 transient 버프가 base로 영구화되는 것을 방지
	OutData.MaxHealth = CaptureASC->GetNumericAttributeBase(UGSAttributeSet::GetMaxHealthAttribute());
	OutData.MaxStamina = CaptureASC->GetNumericAttributeBase(UGSAttributeSet::GetMaxStaminaAttribute());
	OutData.MaxPotionCount = CaptureASC->GetNumericAttributeBase(UGSAttributeSet::GetMaxPotionCountAttribute());
	OutData.AttackPower = CaptureASC->GetNumericAttributeBase(UGSAttributeSet::GetAttackPowerAttribute());
	OutData.Defense = CaptureASC->GetNumericAttributeBase(UGSAttributeSet::GetDefenseAttribute());

	// 무기 상태
	if (AGSGASWeapon* Weapon = GetEquippedWeapon())
	{
		OutData.EquippedWeaponClass = Weapon->GetClass();
	}
	OutData.bCombatEnabled = IsCombatEnabled();

	GSGAS_LOG(LogGSGAS, Log, TEXT("CaptureSaveData: HP=%.1f Stamina=%.1f Potion=%.1f Weapon=%s Combat=%d"),
		OutData.Health, OutData.Stamina, OutData.PotionCount,
		*GetNameSafe(OutData.EquippedWeaponClass), OutData.bCombatEnabled ? 1 : 0);
	return true;
}

bool AGSGASCharacterPlayer::RestoreFromSaveData(const FGSGASPlayerSaveData& Data, bool bFullHeal)
{
	const AGSGASPlayerState* GASPS = GetPlayerState<AGSGASPlayerState>();
	if (!GASPS)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("RestoreFromSaveData: PlayerState is null."));
		return false;
	}

	UAbilitySystemComponent* RestoreASC = GASPS->GetAbilitySystemComponent();
	if (!RestoreASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("RestoreFromSaveData: ASC is null."));
		return false;
	}

	// 1. 기존 무기 정리 (이중 스폰 방지 — 보통 새 possess 시 무기 없음, 방어적)
	DestroyEquippedWeapon();

	// 2. base 스탯 먼저 (현재값 클램프가 Max를 참조하므로 순서 중요)
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetMaxHealthAttribute(), Data.MaxHealth);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetMaxStaminaAttribute(), Data.MaxStamina);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetMaxPotionCountAttribute(), Data.MaxPotionCount);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetAttackPowerAttribute(), Data.AttackPower);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetDefenseAttribute(), Data.Defense);

	// 3. 무기 스폰/장착 — 저장된 전투(손)/비전투(등) 상태 그대로 복원
	if (Data.EquippedWeaponClass)
	{
		SpawnAndEquipWeapon(Data.EquippedWeaponClass, Data.bCombatEnabled);
	}

	// 4. 현재 리소스는 최종 Max 확정 후 클램프하여 복원
	//    bFullHeal(사망 부활)이면 저장값 대신 최대치로 채운다 —
	//    체크포인트가 저HP로 저장돼 있으면 부활 직후 다시 죽기 때문.
	const float FinalMaxHealth = RestoreASC->GetNumericAttribute(UGSAttributeSet::GetMaxHealthAttribute());
	const float FinalMaxStamina = RestoreASC->GetNumericAttribute(UGSAttributeSet::GetMaxStaminaAttribute());
	const float FinalMaxPotion = RestoreASC->GetNumericAttribute(UGSAttributeSet::GetMaxPotionCountAttribute());

	const float RestoredHealth = bFullHeal ? FinalMaxHealth : FMath::Clamp(Data.Health, 0.f, FinalMaxHealth);
	const float RestoredStamina = bFullHeal ? FinalMaxStamina : FMath::Clamp(Data.Stamina, 0.f, FinalMaxStamina);
	const float RestoredPotion = bFullHeal ? FinalMaxPotion : FMath::Clamp(Data.PotionCount, 0.f, FinalMaxPotion);

	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetHealthAttribute(), RestoredHealth);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetStaminaAttribute(), RestoredStamina);
	RestoreASC->SetNumericAttributeBase(UGSAttributeSet::GetPotionCountAttribute(), RestoredPotion);

	GSGAS_LOG(LogGSGAS, Log, TEXT("RestoreFromSaveData: HP=%.1f/%.1f Stamina=%.1f Potion=%.1f Weapon=%s Combat=%d FullHeal=%d"),
		RestoredHealth, FinalMaxHealth, RestoredStamina, RestoredPotion,
		*GetNameSafe(Data.EquippedWeaponClass), Data.bCombatEnabled ? 1 : 0, bFullHeal ? 1 : 0);
	return true;
}

void AGSGASCharacterPlayer::EquipFallbackWeaponIfNeeded()
{
	if (!FallbackWeaponClass || GetEquippedWeapon())
	{
		return;
	}

	// 무기를 직접 줍는 시작 맵에서는 발동시키지 않는다.
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (!NoFallbackLevelName.IsNone() && CurrentLevelName == NoFallbackLevelName.ToString())
	{
		return;
	}

	GSGAS_LOG(LogGSGAS, Warning, TEXT("EquipFallbackWeaponIfNeeded: no save data in %s. Equipping %s."),
		*CurrentLevelName, *GetNameSafe(FallbackWeaponClass));

	// 비전투(등 소켓) 상태로 지급 — 시작 자세는 시작 맵과 동일하게 유지
	SpawnAndEquipWeapon(FallbackWeaponClass, false);
}

FRotator AGSGASCharacterPlayer::GetComboFacingRotation() const
{
	// 1순위: 락온 중이면 타겟 방향
	if (UGSGASTargetingComponent* TargetingComp = FindComponentByClass<UGSGASTargetingComponent>())
	{
		if (TargetingComp->IsLockedOn())
		{
			if (const AActor* Target = TargetingComp->GetLockedTarget())
			{
				FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				Dir.Z = 0.f;
				return Dir.ToOrientationRotator();
			}
		}
	}

	// 2순위: 이동 입력 방향 (카메라 기준 월드 방향 변환)
	if (!LastMovementInput.IsNearlyZero())
	{
		const FRotator ControlYaw(0.f, GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::X);
		const FVector Right   = FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y);
		const FVector MoveDir = (Forward * LastMovementInput.X + Right * LastMovementInput.Y).GetSafeNormal();
		return MoveDir.ToOrientationRotator();
	}

	// 3순위: 현재 방향 유지
	return GetActorRotation();
}

void AGSGASCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

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

		// 약공 (InputID: 1) Mouse Left
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AGSGASCharacterPlayer::GASInputPressed, 1);

		// 강공 (InputID: 2) — Shift 조합
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AGSGASCharacterPlayer::GASInputPressed, 2);

		// 상호작용 (InputID: 3) F
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 3);

		// 회피 (InputID: 4) Space
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 4);

		// 락온 (InputID: 5) Mouse Middle
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 5);
		
		// 전투상태 변화 (InputID: 6) X
		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 6);
		
		// 아이템 사용 (InputID: 7) E
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started,this, &AGSGASCharacterPlayer::GASInputPressed, 7);

		// ESC — 즉시 게임 종료
		if (QuitAction)
		{
			EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &AGSGASCharacterPlayer::OnQuit);
		}
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

void AGSGASCharacterPlayer::OnQuit()
{
	UKismetSystemLibrary::QuitGame(this, Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
}

void AGSGASCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y * (-1));
	}
}

void AGSGASCharacterPlayer::OnInteractSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (OtherActor->Implements<UGSGASInteractInterface>())
	{
		// BP 구현체도 잡으려면 Cast가 아니라 Execute_ 패턴이어야 한다.
		ShowPromptText(IGSGASInteractInterface::Execute_GetInteractPromptText(OtherActor));
		GSGAS_LOG(LogGSGAS, Log, TEXT("Interact target entered: %s"), *OtherActor->GetName());
	}
}

void AGSGASCharacterPlayer::OnInteractSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	if (OtherActor->Implements<UGSGASInteractInterface>())
	{
		// NOTE: 상호작용 대상이 서로 겹쳐 있으면 한쪽에서 나올 때 프롬프트가 지워진다.
		//       현재 전시 레벨에는 대상이 붙어있지 않아 문제되지 않음.
		HidePromptText();
		GSGAS_LOG(LogGSGAS, Log, TEXT("Interact target exited: %s"), *OtherActor->GetName());
	}
}

void AGSGASCharacterPlayer::ShowPromptText(const FText& InText, float Duration)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->ShowPromptText(InText, Duration);
	}
}

void AGSGASCharacterPlayer::HidePromptText()
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->HidePromptText();
	}
}

void AGSGASCharacterPlayer::OnOutOfHealth()
{
	OnDeath();
}

void AGSGASCharacterPlayer::OnDeath()
{
	// 사망 재시작 표식 — 다음 복원에서 체크포인트를 "가득 찬 상태"로 부활시키기 위함.
	// 재시작 UI 경로와 무관하게 동작하도록 사망 시점에 세운다.
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSGASPersistenceSubsystem* Persistence = GI->GetSubsystem<UGSGASPersistenceSubsystem>())
		{
			Persistence->MarkDeathRestart();
		}
	}

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

	// 지연 후 GameOver 위젯 표시
	if (GameOverWidgetClass)
	{
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &AGSGASCharacterPlayer::ShowGameOverWidget, GameOverWidgetDelay, false);
	}

	OnDeathBP();
}

void AGSGASCharacterPlayer::ShowGameOverWidget()
{
	if (!GameOverWidgetClass) return;

	UGSGASGameOverWidget* GameOverWidget = CreateWidget<UGSGASGameOverWidget>(GetWorld(), GameOverWidgetClass);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport(10);
	}
}
