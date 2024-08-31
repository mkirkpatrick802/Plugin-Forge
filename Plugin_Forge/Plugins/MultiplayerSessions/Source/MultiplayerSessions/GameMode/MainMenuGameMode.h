#pragma once

#include "CoreMinimal.h"
#include "TheForge/GameMode/TheForgeGameMode.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class MULTIPLAYERSESSIONS_API AMainMenuGameMode : public ATheForgeGameMode
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Main Menu")
	TSubclassOf<UUserWidget> MainMenuSubclass;
	
};
