#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerMessageText.generated.h"

UCLASS()
class BRINKOFESCAPE_API APlayerMessageText : public AActor
{
	GENERATED_BODY()

protected:
	// Text component that will be set to the previous final message
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Text")
		class UTextRenderComponent* TextRenderComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	APlayerMessageText();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FString GetPlayerMessage(); // Get last message the player received
	void FacePlayer(); // Face the player
	
};
