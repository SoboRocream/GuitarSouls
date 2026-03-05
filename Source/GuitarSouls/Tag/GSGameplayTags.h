// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace GSGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Rolling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_GeneralAction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Parrying);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Parried);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Stunned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_DrinkPotion);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Death);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Unequip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_HitReaction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_BlockingHit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_ParriedHit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_KnockBackHit);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Heavy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Special);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Running);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Air);

	// ==================== Character State (GAS 신규) ====================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Immune);		// 페이즈 전환 중 무적
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Staggered);	// 강인도 소진 시 경직
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Phase2);		// 보스 광폭화

	// ==================== Boss Ability ====================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss_Melee_Swing_Horizontal);	// 횡베기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss_Melee_Thrust);				// 찌르기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Boss_Jump_LeapAttack);			// 도약 공격

	// ==================== Effect ====================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Buff_DamageMultiplier);			// 페이즈2 공격력 증가
	
}
