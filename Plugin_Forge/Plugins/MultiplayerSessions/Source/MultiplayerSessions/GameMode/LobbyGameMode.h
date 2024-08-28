#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API ALobbyGameMode : public AGameMode
{
	
	GENERATED_BODY()

public:

	ALobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	
	UFUNCTION(BlueprintCallable)
	void StartServerTravel();

	UFUNCTION(BlueprintCallable)
	void StopServerTravel();

	void Countdown();
	
	void ExecuteServerTravel();
	
private:
	
	UPROPERTY(EditAnywhere, Category="Lobby Settings")
	TSoftObjectPtr<UWorld> StartingLevel;

	UPROPERTY(EditAnywhere, Category="Lobby Settings")
	int CountdownTime = 5;
	
	UPROPERTY(EditAnywhere, Category="Game Settings")
	int MaxPlayerCount = 2;

	FTimerHandle ServerTravelTimer;

	int CountdownCounter = 0;
};
