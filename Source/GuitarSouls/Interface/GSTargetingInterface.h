
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSTargetingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UGSTargetingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GUITARSOULS_API IGSTargetingInterface
{
	GENERATED_BODY()

public:
	virtual void OnTargeted(bool bTargeted) = 0;
	virtual bool CanBeTargeted() = 0;
};
