#pragma once

#include "CoreMinimal.h"
#include "Interactable_Maser.h"
#include "MainMenuDoor.generated.h"

/**
 * 
 */
UCLASS()
class BRINKOFESCAPE_API AMainMenuDoor : public AInteractable_Maser
{
	GENERATED_BODY()

protected:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainMenuDoor")
		FName LevelName; // Name of the level to open
	
public:
	AMainMenuDoor();
	virtual void OnInteract(APlayerCharacter* playerCharacter) override; // Override interact function
	
	
};
