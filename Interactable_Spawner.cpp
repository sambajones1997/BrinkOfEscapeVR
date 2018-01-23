// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable_Spawner.h"
#include "Pickup_Inventory_Items.h"
#include "PlayerCharacter.h"

AInteractable_Spawner::AInteractable_Spawner()
{
	// Create a scene component that can be moved to mark the spawn location of the item
	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Location"));
	SpawnLocation->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f)); // Centre it before attachment
	SpawnLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// On begin play, if the item is using its editor placed location then move the spawn location to the item otherwise move the item to the spawn location
// Also, hide item so it can not be seen until needed
void AInteractable_Spawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnItem)
	{
		SpawnItem->HideItem();
		if (bUseItemPlacement) SpawnLocation->SetWorldLocation(SpawnItem->GetActorLocation()); // If the item has its own placement, then set the spawn location to the items location
		else SpawnItem->SetActorLocation(SpawnLocation->GetComponentLocation()); // Set the location of the spawn item to the spawn location component
	}
}

// When the player interacts, if the spawner is not locked then show the item,
// Otherwise check if the player has the necessary item
void AInteractable_Spawner::OnInteract(APlayerCharacter* playerCharacter)
{
	if (SpawnItem)
	{
		if (bIsLocked)
		{
			if (CheckInventory(playerCharacter))
			{
				bIsLocked = false; // Spawner is no longer locked
				bUseCustomMessage = false; // No longer needs to use a custom message
				SpawnMyItem(); // Spawn the item if the player has the item needed to unlock it
				SendMessageToPlayer(playerCharacter, "Unlocked"); // Let the player know it is unlocked
			}
			else SendMessageToPlayer(playerCharacter, "It's Locked.");
		}
		else
		{
			if(!bItemSpawned) SpawnMyItem(); // If the item hasn't been spawned, then spawn the item
			else SendMessageToPlayer(playerCharacter, "There is nothing else I can do with this"); // Inform the player that nothing else is needed to be done as the item is spawned
		}
	}
}

// The item requirements were met and must be spawned in the world
// Show the item and mark it as spawned
void AInteractable_Spawner::SpawnMyItem()
{
	SpawnItem->ShowItem(); // Show the item that it is 'spawning'
	bItemSpawned = true; // Item has been spawned
}

// Get a value of true or false whether the player's inventory contains the necessary item
bool AInteractable_Spawner::CheckInventory(APlayerCharacter * playerCharacter)
{
	return playerCharacter->CheckInventory(UnlockItem);
}


