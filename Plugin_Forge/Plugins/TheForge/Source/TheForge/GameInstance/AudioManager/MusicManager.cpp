#include "MusicManager.h"

#include "MusicClipData.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UMusicManager::UMusicManager(): MusicDataTable(nullptr), MusicPlayer(nullptr)
{
	
}

void UMusicManager::Init()
{
	MusicPlayer = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	MusicPlayer->SetActorLabel(TEXT("Music Player"));
	
	MusicLayers.Empty();
	for (int i = 0; i < static_cast<int>(EMusicLayers::LayerCount); i++)
	{
		FString BaseName = TEXT("Music Layer ");
		FString IntAsString = FString::FromInt(i);
		FString FullNameString = BaseName + IntAsString;
		UAudioComponent* NewAudioLayer = NewObject<UAudioComponent>(MusicPlayer, *FullNameString);
		NewAudioLayer->AttachToComponent(MusicPlayer->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewAudioLayer->RegisterComponent();
		
		MusicLayers.Add(NewAudioLayer);
	}
}

void UMusicManager::PlayMusicOnLayer(EMusicLayers MusicLayer, const FString ClipName) const
{
	const FMusicClipData* FoundClip = nullptr;
	if (MusicDataTable)
	{
		for (TArray<FName> RowNames = MusicDataTable->GetRowNames(); const FName& RowName : RowNames)
			if (const FMusicClipData* RowData = MusicDataTable->FindRow<FMusicClipData>(RowName, FString("")))
				if (RowData->MusicClipName == ClipName)
				{
					FoundClip = RowData;
					break;
				}
	}

	if (FoundClip)
	{
		MusicLayers[static_cast<int>(MusicLayer)]->SetSound(FoundClip->AudioCue);
		MusicLayers[static_cast<int>(MusicLayer)]->Play();
	}
}