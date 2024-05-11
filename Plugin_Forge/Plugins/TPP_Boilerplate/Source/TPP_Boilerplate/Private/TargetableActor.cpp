#include "TargetableActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

ATargetableActor::ATargetableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComponent->SetupAttachment(RootComponent);
	SetRootComponent(CapsuleComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	TargetingWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Targeting Widget"));
	TargetingWidget->SetupAttachment(RootComponent);
	TargetingWidget->SetWidgetSpace(EWidgetSpace::Screen);
	TargetingWidget->SetDrawAtDesiredSize(true);
	TargetingWidget->SetVisibility(false);
}

void ATargetableActor::TargetGained()
{
	TargetingWidget->SetVisibility(true);
}

void ATargetableActor::TargetLost()
{
	TargetingWidget->SetVisibility(false);
}
