#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MusicManager.generated.h"

UENUM(BlueprintType)
enum class EMusicLayers : uint8
{
	EML_Layer1 = 0 UMETA(DisplayName = "1st Layer"),
	EML_Layer2 = 1 UMETA(DisplayName = "2nd Layer"),
	EML_Layer3 = 2 UMETA(DisplayName = "3rd Layer"),
	EML_Layer4 = 3 UMETA(DisplayName = "4th Layer"),
	EML_Layer5 = 4 UMETA(DisplayName = "5th Layer"),
	EML_Max = 5 UMETA(DisplayName = "Number of Layers", Hidden),
};

DECLARE_DYNAMIC_DELEGATE(FOnMusicClipFinishedDelegate);

UCLASS(Blueprintable)
class THEFORGE_API UMusicManager : public UObject
{
	GENERATED_BODY()

public:

	UMusicManager();
	void Init();

	// Music Management
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayMusicOnLayerByClipName(EMusicLayers MusicLayer, FString ClipName, UPARAM(DisplayName="Music Clip Finished") FOnMusicClipFinishedDelegate OnMusicClipFinished);

	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayMusicOnLayerByRowName(EMusicLayers MusicLayer, FString RowName, UPARAM(DisplayName="Music Clip Finished") FOnMusicClipFinishedDelegate OnMusicClipFinished);
	
	UFUNCTION(BlueprintCallable, Category="Music")
	void StopAllLayers();

	UFUNCTION(BlueprintCallable, Category="Music")
	void StopLayer(EMusicLayers MusicLayer);

private:
	
public:

	UPROPERTY(EditAnywhere, Category="Data Table")
	UDataTable* MusicDataTable;

private:

	UPROPERTY()
	AActor* MusicPlayer;
	
	TArray<UAudioComponent*> MusicLayers;
};
