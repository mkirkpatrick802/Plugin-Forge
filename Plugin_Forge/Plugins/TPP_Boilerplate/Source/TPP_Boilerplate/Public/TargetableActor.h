#pragma once
#include "CoreMinimal.h"
#include "TargetableInterface.h"
#include "GameFramework/Actor.h"
#include "TargetableActor.generated.h"

UCLASS()
class TPP_BOILERPLATE_API ATargetableActor : public AActor, public ITargetableInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* TargetingWidget;
	
public:	
	ATargetableActor();
	
	virtual void TargetGained() override;
	virtual void TargetLost() override;

private:

	
	
};
