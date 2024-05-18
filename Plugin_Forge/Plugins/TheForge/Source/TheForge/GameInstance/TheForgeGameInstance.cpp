#include "TheForgeGameInstance.h"

void UTheForgeGameInstance::Init()
{
	Super::Init();

	MusicManager = NewObject<UMusicManager>(this, MusicManagerClass);
	MusicManager->Init();
	
	NarrationManager = NewObject<UNarrationManager>(this, NarrationManagerClass);
}
