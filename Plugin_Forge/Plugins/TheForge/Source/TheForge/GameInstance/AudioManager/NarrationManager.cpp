#include "NarrationManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UNarrationManager::UNarrationManager(): NarrationDataTable(nullptr), NarrationPlayer(nullptr) {}

void UNarrationManager::Init()
{
	AActor* TempActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	TempActor->SetActorLabel(TEXT("Narration Player"));

	NarrationPlayer = NewObject<UAudioComponent>(TempActor, FName("Narration Component"));
	NarrationPlayer->AttachToComponent(TempActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	NarrationPlayer->RegisterComponent();

	NarrationPlayer->OnAudioFinished.AddDynamic(this, &UNarrationManager::HandleNarrationClipFinished);
}

void UNarrationManager::PlayNarrationClipByClipName(UPARAM(DisplayName="Narration Clip Finished") const FOnNarrationClipFinishedDelegate OnNarrationClipFinished, const ENarrationClipBehaviors Behavior, const FString ClipName)
{
	const FNarrationClipData* FoundClip = nullptr;
	if (NarrationDataTable)
		for (TArray<FName> RowNames = NarrationDataTable->GetRowNames(); const FName& RowName : RowNames)
			if (const FNarrationClipData* RowData = NarrationDataTable->FindRow<FNarrationClipData>(RowName, FString("")))
				if (RowData->NarrationClipName == ClipName)
				{
					FoundClip = RowData;
					break;
				}

	if (FoundClip)
		PlaceClipInQueue(*FoundClip, Behavior, OnNarrationClipFinished);
}

void UNarrationManager::PlayNarrationClipByRowName(UPARAM(DisplayName="Narration Clip Finished") const FOnNarrationClipFinishedDelegate OnNarrationClipFinished, const ENarrationClipBehaviors Behavior, const FString RowName)
{
	const FNarrationClipData* FoundClip = nullptr;
	if (NarrationDataTable)
		for (TArray<FName> RowNames = NarrationDataTable->GetRowNames(); const FName& DataRowName : RowNames)
			if (DataRowName == RowName)
			{
				FoundClip = NarrationDataTable->FindRow<FNarrationClipData>(DataRowName, FString(""));
				break;
			}

	if (FoundClip)
		PlaceClipInQueue(*FoundClip, Behavior, OnNarrationClipFinished);
}

void UNarrationManager::PlaceClipInQueue(const FNarrationClipData& NarrationClip, const ENarrationClipBehaviors Behavior, const FOnNarrationClipFinishedDelegate& OnNarrationClipFinished)
{
	if (!NarrationPlayer->IsPlaying())
	{
		NarrationPlayer->SetSound(NarrationClip.AudioCue);

		if (OnNarrationClipFinished.IsBound())
			NarrationPlayer->OnAudioFinished.Add(OnNarrationClipFinished);
		
		NarrationPlayer->Play();
		return;
	}

	FNarrationClip Clip;
	Clip.NarrationClipData = NarrationClip;
	Clip.CallbackDelegate = OnNarrationClipFinished;
	
	switch(Behavior)
	{
	case ENarrationClipBehaviors::ENCB_Default:
		{
			NarrationQueue.Enqueue(Clip);
		}
		break;
	case ENarrationClipBehaviors::ENCB_Interrupt:
		{
			NarrationPlayer->Stop();

			if (const FNarrationClipData* CurrentNarrationClip = GetNarrationClipDataFromSource(NarrationPlayer->GetSound()))
			{
				FNarrationClip NewClip;
				NewClip.NarrationClipData = *CurrentNarrationClip;
				NarrationQueue.Enqueue(NewClip);	
			}

			NarrationPlayer->SetSound(NarrationClip.AudioCue);
			NarrationPlayer->Play();
		}
		break;
	case ENarrationClipBehaviors::ENCB_PlaceAtFront:
		{
			TQueue<FNarrationClip> TempQueue;
			TempQueue.Enqueue(Clip);

			while (!NarrationQueue.IsEmpty())
			{
				FNarrationClip TempClip;
				NarrationQueue.Dequeue(TempClip);

				TempQueue.Enqueue(TempClip);
			}

			while (!TempQueue.IsEmpty())
			{
				FNarrationClip TempClip;
				TempQueue.Dequeue(TempClip);

				NarrationQueue.Enqueue(TempClip);
			}
		}
		break;
	}
}

FNarrationClipData* UNarrationManager::GetNarrationClipDataFromSource(const USoundBase* Source) const
{
	if (NarrationDataTable)
		for (TArray<FName> RowNames = NarrationDataTable->GetRowNames(); const FName& RowName : RowNames)
			if (FNarrationClipData* RowData = NarrationDataTable->FindRow<FNarrationClipData>(RowName, FString("")))
				if (RowData->AudioCue == Source)
					return RowData;

	return nullptr;
}

void UNarrationManager::HandleNarrationClipFinished()
{
	FNarrationClip NextClip;
	NarrationQueue.Dequeue(NextClip);

	NarrationPlayer->SetSound(NextClip.NarrationClipData.AudioCue);
	if (NextClip.CallbackDelegate.IsBound())
		NarrationPlayer->OnAudioFinished.Add(NextClip.CallbackDelegate);
	NarrationPlayer->Play();
}
