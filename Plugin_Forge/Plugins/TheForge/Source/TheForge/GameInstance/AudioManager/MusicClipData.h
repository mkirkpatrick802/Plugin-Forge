#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MusicClipData.generated.h"

USTRUCT(BlueprintType)
struct FMusicClipData : public FTableRowBase
{
	GENERATED_BODY();

	FMusicClipData(): AudioCue(nullptr) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MusicClipName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* AudioCue;
};