// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "PlayerCharacter.h"

ADoor::ADoor()
{
	// Construction helper gets the specific mesh set in the TEXT and applies it to the static mesh
	const ConstructorHelpers::FObjectFinder<UStaticMesh> DoorObj(TEXT("/Game/SimpleHouseInteriors/Meshes/SM_Door_02.SM_Door_02"));
	if (DoorObj.Object)InteractableMesh->SetStaticMesh(DoorObj.Object);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EDoorState == EDoorStateEnum::DSE_Opening || EDoorState == EDoorStateEnum::DSE_Closing)
	{
		OpenCloseDoor(DeltaTime); // The door is rotated a small amount every frame until it is fully open or closed
	}
}

// When the player attempts to interact with the door, it checks whether it is locked or not
void ADoor::OnInteract(APlayerCharacter * playerCharacter)
{
	if (bIsDoorLocked) CheckForItem(playerCharacter);
	// If the door is no longer locked but needs power, then the player is informed that the door needs to be powered
	if (bNeedsPower && !bIsDoorLocked)
	{
		if (bIsPowered) ChangeState(); // If power is delivered, change the state of the door
		else SendMessageToPlayer(playerCharacter, "The Door Needs Power");	
	}
	else if (!bNeedsPower && !bIsDoorLocked)	ChangeState(); // Change state of the door if it doesnt need power and is no longer locked
}

// Rotates the door during transition phases using the time between frames
void ADoor::OpenCloseDoor(float& DeltaTime)
{
	fTransitionTime -= DeltaTime;
	if (fTransitionTime <= 0.0f) // When the transition time has finished, the door has transition into a new completed state
	{
		if (EDoorState == EDoorStateEnum::DSE_Opening)
			EDoorState = EDoorStateEnum::DSE_Open;
		else if (EDoorState == EDoorStateEnum::DSE_Closing)
			EDoorState = EDoorStateEnum::DSE_Closed;
		fTransitionTime = MaxTransitionTime;
	}
	else if (fTransitionTime > 0.0f) // The transition is still being animated so rotation is added
	{
		if(EDoorState == EDoorStateEnum::DSE_Opening) AddActorLocalRotation(FRotator(0.0f, 100 * DeltaTime, 0.0f));
		else if (EDoorState == EDoorStateEnum::DSE_Closing) AddActorLocalRotation(FRotator(0.0f, -100 * DeltaTime, 0.0f));
	}

}

// Locked door boolean flag is set to false -- Door can be opened
void ADoor::UnlockDoor()
{
	bIsDoorLocked = false;
}

// Calls the player's function to check whether they have the unlock item or not
void ADoor::CheckForItem(APlayerCharacter * playerCharacter)
{
	bool bHasItem = playerCharacter->CheckInventory(DoorUnlockItem);
	if (bHasItem)
	{
		bUseCustomMessage = false; // A custom message is no longer being used
		UnlockDoor(); // Door is unlocked
		SendMessageToPlayer(playerCharacter, "Door Unlocked"); // Door is now unlocked and the player is informed
	}
	else SendMessageToPlayer(playerCharacter, "Door Locked"); // Inform the player that the door is currently locked
}

// Depending on the current state of interaction, it needs to be swapped and the transition time calculated when interacted with
void ADoor::ChangeState()
{
	switch (EDoorState)
	{
	case EDoorStateEnum::DSE_Closed: // If the door is closed, it begins to open using the max length of transition timer
		EDoorState = EDoorStateEnum::DSE_Opening;
		fTransitionTime = MaxTransitionTime;
		break;
	case EDoorStateEnum::DSE_Closing: // If the door is closing then we need to reopen the door and calculate how long to do so for 
		EDoorState = EDoorStateEnum::DSE_Opening;
		fTransitionTime = 1 - fTransitionTime; // Door was already part way through transition, remaining time is calculated
		break;
	case EDoorStateEnum::DSE_Opening:
		EDoorState = EDoorStateEnum::DSE_Closing; // If the door is opening, then we need to start closing it
		fTransitionTime = 1 - fTransitionTime;// Door was already part way through transition, remaining time is calculated
		break;
	case EDoorStateEnum::DSE_Open: // If the door is open, then begin to close it using max transition time
		EDoorState = EDoorStateEnum::DSE_Closing;
		fTransitionTime = MaxTransitionTime;
		break;
	}
}