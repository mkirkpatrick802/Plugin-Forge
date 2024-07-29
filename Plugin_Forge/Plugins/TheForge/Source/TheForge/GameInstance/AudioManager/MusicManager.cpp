#include "MusicManager.h"

#include "MusicClipData.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UMusicManager::UMusicManager(): MusicDataTable(nullptr), MusicPlayer(nullptr) {}

void UMusicManager::Init()
{
	MusicPlayer = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	MusicPlayer->SetActorLabel(TEXT("Music Player"));
	
	MusicLayers.Empty();
	for (int i = 0; i < static_cast<int>(EMusicLayers::EML_Max); i++)
	{
		FString BaseName = TEXT("Music Layer ");
		FString IntAsString = FString::FromInt(i);
		FString FullNameString = BaseName + IntAsString;
		UAudioComponent* NewAudioLayer = NewObject<UAudioComponent>(MusicPlayer, *FullNameString);
		NewAudioLayer->AttachToComponent(MusicPlayer->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewAudioLayer->RegisterComponent();
		NewAudioLayer->bIsMusic = true;
		
		MusicLayers.Add(NewAudioLayer);
	}
}

void UMusicManager::PlayMusicOnLayerByClipName(EMusicLayers MusicLayer, const FString ClipName, const FOnMusicClipFinishedDelegate OnMusicClipFinished)
{
	const int LayerNumber = static_cast<int>(MusicLayer);
	const FMusicClipData* FoundClip = nullptr;
	
	if (MusicDataTable)
		for (TArray<FName> RowNames = MusicDataTable->GetRowNames(); const FName& RowName : RowNames)
			if (const FMusicClipData* RowData = MusicDataTable->FindRow<FMusicClipData>(RowName, FString("")))
				if (RowData->MusicClipName == ClipName)
				{
					FoundClip = RowData;
					break;
				}

	UAudioComponent* Layer = MusicLayers[LayerNumber];
	if (FoundClip)
	{
		Layer->SetSound(FoundClip->AudioCue);
		
		if (OnMusicClipFinished.IsBound())
			Layer->OnAudioFinished.Add(OnMusicClipFinished);
		
		Layer->Play();
	}
}

void UMusicManager::PlayMusicOnLayerByRowName(EMusicLayers MusicLayer, const FString RowName, const FOnMusicClipFinishedDelegate OnMusicClipFinished)
{
	const int LayerNumber = static_cast<int>(MusicLayer);
	const FMusicClipData* FoundClip = nullptr;
	
	if (MusicDataTable)
		for (TArray<FName> RowNames = MusicDataTable->GetRowNames(); const FName& DataRowName : RowNames)
			if (DataRowName == RowName)
			{
				FoundClip = MusicDataTable->FindRow<FMusicClipData>(DataRowName, FString(""));
				break;
			}

	UAudioComponent* Layer = MusicLayers[LayerNumber];
	if (FoundClip)
	{
		Layer->SetSound(FoundClip->AudioCue);

		if (OnMusicClipFinished.IsBound())
			Layer->OnAudioFinished.Add(OnMusicClipFinished);
		
		Layer->Play();
	}
}

void UMusicManager::StopAllLayers()
{
	for (const auto Layer : MusicLayers)
		Layer->Stop();
}

void UMusicManager::StopLayer(EMusicLayers MusicLayer)
{
	MusicLayers[static_cast<int>(MusicLayer)]->Stop();
}