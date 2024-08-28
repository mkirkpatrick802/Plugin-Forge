#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameMode::ALobbyGameMode()
{
	CountdownCounter = CountdownTime;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num(); NumberOfPlayers == MaxPlayerCount)
		StartServerTravel();
}

void ALobbyGameMode::StartServerTravel()
{
	GetWorldTimerManager().SetTimer(ServerTravelTimer, this, &ALobbyGameMode::Countdown, 1, true);
}

void ALobbyGameMode::StopServerTravel()
{
	GetWorldTimerManager().ClearTimer(ServerTravelTimer);
}

void ALobbyGameMode::Countdown()
{
	CountdownCounter--;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("Game Starting in: %d"), CountdownCounter));

	if(CountdownCounter == 0)
	{
		ExecuteServerTravel();
		GetWorldTimerManager().ClearTimer(ServerTravelTimer);
	}
}

void ALobbyGameMode::ExecuteServerTravel()
{
	const FSoftObjectPath LevelObjectPath = StartingLevel.ToSoftObjectPath();
	FString FilePath = LevelObjectPath.ToString();

	if (int DotIndex; FilePath.FindLastChar('.', DotIndex))
		FilePath = FilePath.Left(DotIndex);

	if (const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num(); NumberOfPlayers != 1)
	{
		FilePath.Append(TEXT("?listen"));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Loading Level: %s"), *FilePath));
	
		if(UWorld* World = GetWorld())
		{
			for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
				if(StreamingLevel)
				{
					StreamingLevel->SetShouldBeLoaded(false);
					StreamingLevel->SetIsRequestingUnloadAndRemoval(true);
				}

			World->FlushLevelStreaming(EFlushLevelStreamingType::Full);

			/*
			 *	May be crashing ue5 when packaged build
			 *	Further testing required
			 */
			
			bUseSeamlessTravel = true;
			World->ServerTravel(*FilePath);
		}
	}
	else
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(),StartingLevel);
	}
}
