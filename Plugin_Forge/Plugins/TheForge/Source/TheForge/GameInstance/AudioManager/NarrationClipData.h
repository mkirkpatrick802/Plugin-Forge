#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NarrationClipData.generated.h"

USTRUCT(BlueprintType)
struct FNarrationClipData : public FTableRowBase
{
	GENERATED_BODY();

	FNarrationClipData(): AudioCue(nullptr) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NarrationClipName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* AudioCue;
};