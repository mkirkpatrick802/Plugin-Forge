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
	
private:
	
public:

	UPROPERTY(EditAnywhere, Category="Audio Manager")
	TSubclassOf<UMusicManager> MusicManagerClass;

	UPROPERTY(EditAnywhere, Category="Audio Manager")
	TSubclassOf<UNarrationManager> NarrationManagerClass;

	UPROPERTY(BlueprintReadOnly, Category="Music Manager")
	UMusicManager* MusicManager;

	UPROPERTY(BlueprintReadOnly, Category="Narration Manager")
	UNarrationManager* NarrationManager;
	
private:
};
