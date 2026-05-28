// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace GSGASGameplayTags
{
	// ── Character State ────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_ComboWindow);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Parrying);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Parried);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_AttackCollisionActive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Invincible);
 
	// ── Character Action ───────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_HitReaction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_UsePotion);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Heavy);
 
	// ── Data ──────────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_StaminaCost);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_HealAmount);

	// ── AI State ──────────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Combat);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Patrol);

	// ── AI Event ──────────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Event_TargetAcquired);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Event_Alert);

	// ── Boss ──────────────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Phase_2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_State_PhaseTransition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Attack);

	// ── Combat Toggle ─────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_CombatEnabled);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_CombatType_Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_CombatType_TwoHanded);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_CombatType_Fist);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_EquipWeapon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_UnequipWeapon);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Toggling);
}