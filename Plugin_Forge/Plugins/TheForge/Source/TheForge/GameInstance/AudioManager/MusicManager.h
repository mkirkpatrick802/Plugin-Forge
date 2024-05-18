﻿#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MusicManager.generated.h"

UENUM(BlueprintType)
enum class EMusicLayers : uint8
{
	Layer1 = 0 UMETA(DisplayName = "1st Layer"),
	Layer2 = 1 UMETA(DisplayName = "2nd Layer"),
	Layer3 = 2 UMETA(DisplayName = "3rd Layer"),
	Layer4 = 3 UMETA(DisplayName = "4th Layer"),
	Layer5 = 4 UMETA(DisplayName = "5th Layer"),
	LayerCount = 5 UMETA(DisplayName = "Number of Layers", Hidden),
};

UCLASS(Blueprintable)
class THEFORGE_API UMusicManager : public UObject
{
	GENERATED_BODY()

public:

	UMusicManager();
	void Init();
	
	UFUNCTION(BlueprintCallable)
	void PlayMusicOnLayer(EMusicLayers MusicLayer, FString ClipName) const;
	
public:

	UPROPERTY(EditAnywhere, Category="Data Table")
	UDataTable* MusicDataTable;

private:

	UPROPERTY()
	AActor* MusicPlayer;
	
	TArray<UAudioComponent*> MusicLayers;
};
