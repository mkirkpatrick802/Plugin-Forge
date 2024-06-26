#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num(); NumberOfPlayers == 2)
	{
		if(UWorld* World = GetWorld())
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/TopDown_Test/Maps/TopDownPlayer?listen"));
		}
	}
}
