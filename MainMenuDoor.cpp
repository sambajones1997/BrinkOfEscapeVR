#include "MainMenuDoor.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "PlayerCharacter.h"
#include "Classes/Kismet/GameplayStatics.h"

AMainMenuDoor::AMainMenuDoor()
{
	// Construction helper gets the specific mesh set in the TEXT and applies it to the static mesh
	const ConstructorHelpers::FObjectFinder<UStaticMesh> DoorObj(TEXT("/Game/SimpleHouseInteriors/Meshes/SM_Door_02.SM_Door_02"));
	if (DoorObj.Object)InteractableMesh->SetStaticMesh(DoorObj.Object);
}

// When the player attempts to interact with the door, it checks whether it is locked or not
void AMainMenuDoor::OnInteract(APlayerCharacter * playerCharacter)
{
	// Open the level that is stored in level name
	FLatentActionInfo LatentInfo;
	UGameplayStatics::OpenLevel(this, LevelName, true);
}



