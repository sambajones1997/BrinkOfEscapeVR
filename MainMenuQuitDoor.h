#pragma once

#include "CoreMinimal.h"
#include "Interactable_Maser.h"
#include "MainMenuQuitDoor.generated.h"

/**
 * 
 */
UCLASS()
class BRINKOFESCAPE_API AMainMenuQuitDoor : public AInteractable_Maser
{
	GENERATED_BODY()
public:
	AMainMenuQuitDoor();
	virtual void OnInteract(APlayerCharacter* playerCharacter) override;
	
};
