#pragma once
#include "CoreMinimal.h"
#include "AudioManager/MusicManager.h"
#include "AudioManager/NarrationManager.h"
#include "Engine/GameInstance.h"
#include "TheForgeGameInstance.generated.h"

UCLASS()
class THEFORGE_API UTheForgeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;
	
	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& InMapName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

private:
	
	void StartAudioSystem();
	
public:

	UPROPERTY(EditAnywhere, Category="Audio Manager")
	TSubclassOf<AMusicManager> MusicManagerClass;

	UPROPERTY(EditAnywhere, Category="Audio Manager")
	TSubclassOf<ANarrationManager> NarrationManagerClass;

	UPROPERTY(BlueprintReadOnly, Category="Music Manager")
	AMusicManager* MusicManager;

	UPROPERTY(BlueprintReadOnly, Category="Narration Manager")
	ANarrationManager* NarrationManager;
	
private:
	
};
