#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NarrationManager.generated.h"

UCLASS(Blueprintable)
class THEFORGE_API UNarrationManager : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Clips")
	UDataTable* NarrationDataTable;
};
