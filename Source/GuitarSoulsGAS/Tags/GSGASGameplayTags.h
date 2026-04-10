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
 
	// ── Character Action ───────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_HitReaction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Roll);
 
	// ── Data ──────────────────────────────────────────────────────────────────
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_StaminaCost);
}