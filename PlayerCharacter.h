// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BRINKOFESCAPE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	// A camera component is used to let the game be rendered from a certain perspective, this is the player's
	UPROPERTY(EditAnywhere, Category = "Camera")
		class UCameraComponent* FirstPersonCamera;

	// A Flashlight will be simulated using a simple spot light component
	UPROPERTY(EditAnywhere, Category = "Flashlight")
		class USpotLightComponent* Flashlight;

	// The intensity of the light the flashlight should produce
	UPROPERTY(EditAnywhere,  BlueprintReadWrite, Category = "Flashlight")
		float FlashlightIntensity = 2500.0f;

	// Whether the flashlight is enabled for the player to use or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
		bool bFlashlightEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		bool bIsUsingHMD = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		bool bIsTeleporting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		bool bTeleportationIsValid = false;

	// The speed (number of units) the player can move in one second. 1 unit = 1 cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Movement")
		float MovementSpeed = 250.0f;

	// A Dynamic list of items that can be stored by the player, uses the highest available class to support multiple item types
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TArray<AActor*> Inventory;

	// Stores the last interactable item raytraced so that it can be used easily without needing to raytrace for it again
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interactable")
		class AInteractable_Maser* InteractableItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class USceneComponent* VRBaseComponent; // Creates a scene component we will use for the root of the VR Controllers

	// Left and Right Motion Controllers, respectively
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class UMotionControllerComponent* MC_Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class UMotionControllerComponent* MC_Right;

	// Flashlight static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class UStaticMeshComponent* SM_Flashlight;

	// Hand static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class UStaticMeshComponent* SM_Hand_R;

	// Mesh to be used for displaying the teleporter
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class UStaticMeshComponent* SM_TeleportMesh;

	// Spawn player message to world component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR")
		class USpawnPlayerMessage* SpawnMessageComponent;

	// Material Instance that is dynamic so teleport can change colour for valid / invalid
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInstanceDynamic* TeleportInstancedMaterial;

	// The last message the player received in string from from an object
	UPROPERTY(VisibleAnywhere, Category = "Interactable")
		FString lastMessage;

	// The time between line / ray traces
	float LineTraceTimer = 0.05f;
	// The time between storing and clearing the last received message variable
	float fTextTimer = 0.0f;
	float MaxTextTimer = 2.0f;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Toggle's the flashlight on and off
	UFUNCTION(BlueprintCallable)
	void ToggleFlashlight();
	// Move Forward with an axis value of -1 to 1
	UFUNCTION(BlueprintCallable)
	void MoveForward(float axis);
	// Move Right with an axis value of -1 to 1
	UFUNCTION(BlueprintCallable)
	void MoveRight(float axis);
	void ReturnToMainMenu();
	UFUNCTION(BlueprintCallable)
	void MoveTeleportLocation();
	UFUNCTION(BlueprintCallable)
	void Teleport();
	UFUNCTION(BlueprintCallable)
	void ActivateTeleportationMethod();

public:	

	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when the player wants to interact with an object, if they are looking at a valid interactable one
	UFUNCTION(BlueprintCallable)
	void Interact();
	// Checks to see if the player is storing a specific item in their inventory
	UFUNCTION(BlueprintCallable)
	bool CheckInventory(AActor* item);
	// Adds a reference to a specific item into the players dynamic inventory
	UFUNCTION(BlueprintCallable)
	void AddItemToIventory(AActor* item);
	// Removes an item from the inventory using a reference to the item being removed
	UFUNCTION(BlueprintCallable)
	void RemoveItemInInvetory(AActor* item);
	// Removes an item from the inventroy list at a specific index
	UFUNCTION(BlueprintCallable)
	void RemoveItemInInventoryAtIndex(int Index);
	// Sends a message to the player's LastMessage variable which is displayed on screen
	UFUNCTION(BlueprintCallable)
	void SendMessageToPlayer(FString Text);
	// Can be called from blueprints, gets the player's last received message
	UFUNCTION(BlueprintCallable, Category = "Controls")
	FString GetPlayerMessage();
	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
		class UStaticMeshComponent* GetPlayerHand();
};
