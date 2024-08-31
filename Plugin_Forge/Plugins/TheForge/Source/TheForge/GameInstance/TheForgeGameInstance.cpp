#include "TheForgeGameInstance.h"

#include "MoviePlayer.h"
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"

void UTheForgeGameInstance::Init()
{
	Super::Init();

	StartAudioSystem();
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UTheForgeGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTheForgeGameInstance::EndLoadingScreen);
	
}

void UTheForgeGameInstance::StartAudioSystem()
{
	MusicManager = GetWorld()->SpawnActor<AMusicManager>(AMusicManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	MusicManager->SetLifeSpan(0);
	MusicManager->Init();
	
	NarrationManager = GetWorld()->SpawnActor<ANarrationManager>(ANarrationManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	NarrationManager->SetLifeSpan(0);
	NarrationManager->Init();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Blue,
			FString::Printf(TEXT("Audio Systems Started!!")));
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