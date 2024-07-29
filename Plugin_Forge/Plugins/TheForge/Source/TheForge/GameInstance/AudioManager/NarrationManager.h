#pragma once
#include "CoreMinimal.h"
#include "NarrationClipData.h"
#include "UObject/Object.h"
#include "NarrationManager.generated.h"

UENUM(BlueprintType)
enum class ENarrationClipBehaviors : uint8
{
	ENCB_Default UMETA(DisplayName = "Default"),
	ENCB_Interrupt UMETA(DisplayName = "Interrupt"),
	ENCB_PlaceAtFront UMETA(DisplayName = "Place At Front"),
	ENCB_Max UMETA(DisplayName = "Behavior Count", Hidden)
};

DECLARE_DYNAMIC_DELEGATE(FOnNarrationClipFinishedDelegate);

struct FNarrationClip
{
	FNarrationClipData NarrationClipData;
	FOnNarrationClipFinishedDelegate CallbackDelegate;	
};

UCLASS(Blueprintable)
class THEFORGE_API UNarrationManager : public UObject
{
	GENERATED_BODY()

public:

	UNarrationManager();
	void Init();

	// Narration Management
	UFUNCTION(BlueprintCallable, Category="Narration")
	void PlayNarrationClipByClipName(UPARAM(DisplayName="Narration Clip Finished") FOnNarrationClipFinishedDelegate OnNarrationClipFinished, ENarrationClipBehaviors Behavior = ENarrationClipBehaviors::ENCB_Default, FString ClipName = "Clip Name");

	UFUNCTION(BlueprintCallable, Category="Narration")
	void PlayNarrationClipByRowName(UPARAM(DisplayName="Narration Clip Finished") FOnNarrationClipFinishedDelegate OnNarrationClipFinished, ENarrationClipBehaviors Behavior = ENarrationClipBehaviors::ENCB_Default, FString RowName = "Row Name");

private:

	void PlaceClipInQueue(const FNarrationClipData& NarrationClip, ENarrationClipBehaviors Behavior, const FOnNarrationClipFinishedDelegate& OnNarrationClipFinished);

	FNarrationClipData* GetNarrationClipDataFromSource(const USoundBase* Source) const;
	
	UFUNCTION()
	void HandleNarrationClipFinished();
	
public:

	UPROPERTY(EditAnywhere, Category="Clips")
	UDataTable* NarrationDataTable;

private:

	UPROPERTY()
	UAudioComponent* NarrationPlayer;
	
	TQueue<FNarrationClip> NarrationQueue;
};
