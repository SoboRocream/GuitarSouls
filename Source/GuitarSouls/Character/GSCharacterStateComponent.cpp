

#include "Character/GSCharacterStateComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UGSCharacterStateComponent::UGSCharacterStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


bool UGSCharacterStateComponent::IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const
{
	return TagsToCheck.HasTagExact(CurrentState);
}

void UGSCharacterStateComponent::ToggleMovementInput(bool bEnabled, float Duration)
{
	if (bEnabled)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = "MovementInputEnableAction";
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = 0;
		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), Duration, LatentInfo);
	}
	else
	{
		bMovementInputEnabled = false;
	}
}

void UGSCharacterStateComponent::ClearState()
{
	if (CurrentState == GSGameplayTags::Character_State_Death)
	{
		return;
	}
	
	CurrentState = FGameplayTag::EmptyTag;
}

void UGSCharacterStateComponent::MovementInputEnableAction()
{
	bMovementInputEnabled = true;
	ClearState();
}
