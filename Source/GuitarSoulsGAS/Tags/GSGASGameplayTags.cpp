// Fill out your copyright notice in the Description page of Project Settings.

#include "Tags/GSGASGameplayTags.h"

namespace GSGASGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_ComboWindow,           "Character.State.ComboWindow");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Parrying,              "Character.State.Parrying");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Parried,               "Character.State.Parried");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Death,                 "Character.State.Death");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_AttackCollisionActive, "Character.State.AttackCollisionActive");
 
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_HitReaction,          "Character.Action.HitReaction");
 
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage,                           "Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_StaminaCost,                      "Data.StaminaCost");
}