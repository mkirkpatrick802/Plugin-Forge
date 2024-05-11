#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTargetableInterface : public UInterface
{
	GENERATED_BODY()
};

class TPP_BOILERPLATE_API ITargetableInterface
{
	GENERATED_BODY()

public:
	
	virtual void TargetGained() = 0;
	virtual void TargetLost() = 0;
};