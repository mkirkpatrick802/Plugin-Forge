#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(const int32 NumberOfPublicConnection, const FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnection;

	if(const UGameInstance* GameInstance = GetGameInstance())
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &UMenu::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if(!UUserWidget::Initialize()) return false;

	if(HostButton)
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);

	if(JoinButton)
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);

	ConnectionTest();
	GetWorld()->GetTimerManager().SetTimer(ConnectionTimerHandle, this, &UMenu::ConnectionTest, ConnectionTestCycle, true);
	
	return true;
}

void UMenu::NativeDestruct()
{
	Super::NativeDestruct();
	
	MenuTearDown();
	GetWorld()->GetTimerManager().ClearTimer(ConnectionTimerHandle);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				15.f,
				FColor::Green,
				FString(TEXT("Session Created Successfully"))
			);
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}

		SessionCreated = true;
	}
	else
	{
		if (SessionCreated) return;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to Create Session"))
			);
		}

		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) return;

	const FString Worked = bWasSuccessful ? "Success" : "Failed";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			bWasSuccessful ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("Finding Session: %s"), *Worked)
		);
	}
	
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if(SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if(!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	const FString Worked = bWasSuccessful ? "Success" : "Failed";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			bWasSuccessful ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("Starting Session: %s"), *Worked)
		);
	}
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	if(const UWorld* World = GetWorld())
	{
		if(APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			const FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

void UMenu::ConnectionTest() const
{
	if(MultiplayerSessionsSubsystem->IsSteamConnected())
	{
		GEngine->AddOnScreenDebugMessage(123, 5.f, FColor::Green, TEXT("User is connected to Steam!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(123, 5.f, FColor::Red, TEXT("User is not connected to Steam!"));
	}
}
