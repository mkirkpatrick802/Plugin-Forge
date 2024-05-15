#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Main Menu")
	TSubclassOf<UUserWidget> MainMenuSubclass;
	
};
