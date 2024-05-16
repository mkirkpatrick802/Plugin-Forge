#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPPController.generated.h"

UCLASS()
class TPP_BOILERPLATE_API ATPPController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
	FORCEINLINE bool GetMouseCursorVisibility() const { return GetMouseCursor() != EMouseCursor::None; }

};
