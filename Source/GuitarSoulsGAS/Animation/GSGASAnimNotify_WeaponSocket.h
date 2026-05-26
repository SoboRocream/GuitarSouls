// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GSGASAnimNotify_WeaponSocket.generated.h"

/**
 * Toggle Combat 몽타주의 특정 프레임에 배치
 * - bEquip = true  : 무기를 손(EquipSocket)으로 이동   (장착 모션 중)
 * - bEquip = false : 무기를 등(UnEquipSocket)으로 이동 (해제 모션 중)
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASAnimNotify_WeaponSocket : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

	// true = EquipSocket(손), false = UnEquipSocket(등)
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bEquip = true;
};
