#include "TheForgeGameInstance.h"

#include "MoviePlayer.h"

void UTheForgeGameInstance::Init()
{
	Super::Init();

	MusicManager = NewObject<UMusicManager>(this, MusicManagerClass);
	MusicManager->Init();
	
	NarrationManager = NewObject<UNarrationManager>(this, NarrationManagerClass);
	NarrationManager->Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UTheForgeGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTheForgeGameInstance::EndLoadingScreen);
}

void UTheForgeGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UTheForgeGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	
}
