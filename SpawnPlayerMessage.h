#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnPlayerMessage.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRINKOFESCAPE_API USpawnPlayerMessage : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnPlayerMessage();

	UFUNCTION(BlueprintCallable)
	void SpawnPlayerMessage(FVector actorLocation, FVector actorForwardVector);
	
};
