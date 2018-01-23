#include "SpawnPlayerMessage.h"
#include "PlayerMessageText.h"
#include "kismet/GameplayStatics.h"
#include "Classes/Engine/World.h"

// Sets default values for this component's properties
USpawnPlayerMessage::USpawnPlayerMessage()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void USpawnPlayerMessage::SpawnPlayerMessage(FVector actorLocation, FVector actorForwardVector)
{
	FVector spawnLocation = actorLocation + (actorForwardVector * 25.0f);
	UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *spawnLocation.ToString());
	GetWorld()->SpawnActor<APlayerMessageText>(APlayerMessageText::StaticClass(), spawnLocation, (FRotator(0.0f, 0.0f, 0.0f)), FActorSpawnParameters::FActorSpawnParameters());
}
