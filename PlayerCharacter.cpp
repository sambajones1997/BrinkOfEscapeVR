// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h" // Include the header file for player character
#include "Classes/Camera/CameraComponent.h" // Allow access to camera component
#include "Classes/Components/StaticMeshComponent.h" // Allow access to static mesh components
#include "Classes/Components/CapsuleComponent.h" // Allow access to capsule components
#include "Classes/Components/SpotLightComponent.h" // Allow access to spotlight components -- light used for flashlight
#include "Classes/Components/InputComponent.h" // Allow access to the input component
#include "Interactable_Maser.h" // Allow access to the master file for interactable items
#include "Classes/Kismet/GameplayStatics.h" // Allow access to gameplay static functions
#include "MotionControllerComponent.h" // Use the motion controller component libraries
#include "HeadMountedDisplay.h" // Use the head mounted display libraries
#include "HeadMountedDisplayFunctionLibrary.h" // HMD function libraries
#include "ConstructorHelpers.h" // Allow use of constructor helpers to get and set objects
#include "Classes/Materials/MaterialInstanceDynamic.h" // Allow use of dynamic material instances
#include "Classes/Materials/MaterialInterface.h" // Allow use of the material interface
#include "SpawnPlayerMessage.h" // Allow use of the spawn player message component

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create & Set up first person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));

	// Create & Set up the Motion Controller components
	// Used as a base for all the VR needs
	VRBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VR_BaseComponent"));
	VRBaseComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	VRBaseComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	VRBaseComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	// Creating the instanced material for the teleport location
	TeleportInstancedMaterial = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("NewTeleportInstancedMaterial")); // Create the dynamic material instance object
	// Create the Left and Right motion controller components and attaching them to the base vr component
	// Left Controller
	MC_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Motion Controller_Left"));
	MC_Left->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MC_Left->AttachToComponent(VRBaseComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// Right Controller
	MC_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Motion Controller_Right"));
	MC_Left->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MC_Right->AttachToComponent(VRBaseComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// Creating the flashlight static mesh
	SM_Flashlight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Flashlight_L"));
	SM_Flashlight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SM_Flashlight->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	SM_Flashlight->AttachToComponent(MC_Left, FAttachmentTransformRules::KeepRelativeTransform);
	// Creating the static mesh for the right hand
	SM_Hand_R = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Hand_R"));
	SM_Hand_R->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SM_Hand_R->AttachToComponent(MC_Right, FAttachmentTransformRules::KeepRelativeTransform);
	SM_Hand_R->SetCollisionProfileName("OverlapAll"); // Hand will trigger overlap collision
	SM_Hand_R->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin); // What to do when the hand begins an overlap
	SM_Hand_R->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd); // What to do when the hand ends an overlap
	// Create the static mesh to indicate teleport location
	SM_TeleportMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Base_TeleportMesh"));
	SM_TeleportMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SM_TeleportMesh->SetCollisionProfileName("NoCollision"); // Teleport Mesh will not be colliding with anything
	SM_TeleportMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SM_TeleportMesh->SetVisibility(false, true); // Teleport mesh is invisible until in use
	// Create & set up flashlight component
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light_Flashlight_L"));
	Flashlight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Flashlight->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	Flashlight->AttachToComponent(SM_Flashlight, FAttachmentTransformRules::KeepRelativeTransform);
	Flashlight->SetInnerConeAngle(15.0f); // The brightest spot of the light
	Flashlight->SetOuterConeAngle(25.0f); // The fade out angle of the light
	Flashlight->SetAttenuationRadius(4000.0f); // The distance the light will travel
	Flashlight->SetIntensity(0.0f); // The intensity (brightness) of the light
	
	InteractableItem = CreateDefaultSubobject<AInteractable_Maser>(TEXT("InteractableItem"));
	SpawnMessageComponent = CreateDefaultSubobject<USpawnPlayerMessage>(TEXT("Spawn Message Component"));
	// Getting and setting the static meshes using constructors
	// Getting the torch static mesh
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TorchObj(TEXT("/Game/CustomSimpleHouseInteriors/Meshes/SM_Torch_1.SM_Torch_1"));
	if (TorchObj.Object)SM_Flashlight->SetStaticMesh(TorchObj.Object); // If the object is valid, set the static mesh of the torch

	// Getting the hand static mesh
	const ConstructorHelpers::FObjectFinder<UStaticMesh> HandObj(TEXT("/Game/CustomSimpleHouseInteriors/Meshes/SM_Hand.SM_Hand"));
	if (HandObj.Object)SM_Hand_R->SetStaticMesh(HandObj.Object); // If successful, set the hand static mesh to the object

	// Getting the teleport mesh 
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TeleportMesh(TEXT("/Game/CustomSimpleHouseInteriors/Meshes/SM_TeleportMesh.SM_TeleportMesh"));
	if (TeleportMesh.Object)SM_TeleportMesh->SetStaticMesh(TeleportMesh.Object); // If valid, set the static mesh

	// Setting up the instanced material for the teleporter so the colour can be changed to signify valid and invalid locations.
	UMaterialInterface* teleportMaterial = SM_TeleportMesh->GetMaterial(0); // Create a material interface for the teleport static mesh material
	TeleportInstancedMaterial = UMaterialInstanceDynamic::Create(teleportMaterial, nullptr); // Createa a dynamic instance of the material
	SM_TeleportMesh->SetMaterial(0, TeleportInstancedMaterial); // Set the material of the teleport static mesh to the new dynamic instance (so it can change)
	GetCapsuleComponent()->SetCapsuleRadius(50.0f, true); // Set the capsule radius to be larger as we are incorporating the player
	FlashlightIntensity = 5000.0f; // Resets flashlight intensity to base value
	InteractableItem = nullptr; // Current interactable item is empty so we use a nullptr to ensure when checking if nullptr or not, we get the correct answer
	bFlashlightEnabled = false; // Flashlight is not enabled at the beginning
	MC_Left->Hand = EControllerHand::Left; // letting UE4 know which motion controller component is tracking which controller
	MC_Right->Hand = EControllerHand::Right;


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	BaseEyeHeight = 90.0f; // Base eye height is the spawn height of the camera
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor); // Set the tracking origin to the floor, this allows Rift users to have the correct height
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsTeleporting) MoveTeleportLocation(); // If using the teleport locomotion, then move the teleport mesh to where the player is pointing
	if (fTextTimer > 0.0f) fTextTimer -= DeltaTime; // Reduce text timer until last message is deleted
	else lastMessage = "";
}

// Called to bind functionality to input
// Input component 'listens' for input signals and sees if they are using them or not
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind axis values that range from -1 to 1 such as movement to the player's input component
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// Bind action mappings to player input component
	PlayerInputComponent->BindAction("ToggleFlashlight", IE_Released, this, &APlayerCharacter::ToggleFlashlight);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &APlayerCharacter::Interact); // Changed for VR
	PlayerInputComponent->BindAction("Escape", IE_Released, this, &APlayerCharacter::ReturnToMainMenu);
	PlayerInputComponent->BindAction("TeleportMove", IE_Pressed, this, &APlayerCharacter::ActivateTeleportationMethod);
	PlayerInputComponent->BindAction("TeleportMove", IE_Released, this, &APlayerCharacter::Teleport);
}

// Toggle flashlight on and off, changing intesnity of light and flipping boolean
void APlayerCharacter::ToggleFlashlight()
{
	bFlashlightEnabled = !bFlashlightEnabled; // Toggle flashlight
	if (bFlashlightEnabled)	Flashlight->SetIntensity(FlashlightIntensity); // Set new intensity of flashlight or set to 0 for off
	else Flashlight->SetIntensity(0.0f);
}

// Move the player forward in the world adding movement input to their movement component
void APlayerCharacter::MoveForward(float axis)
{
	FVector movementVector = FirstPersonCamera->GetForwardVector(); // Get the forward vector of the camera
	movementVector.Z = 0.0f; // do not allow any movement in the z (up) axis
	AddMovementInput(movementVector * MovementSpeed, axis); // Add movement input to the character movement
}

// Move the player Sideways (Strafe) in the world adding movement input to their movement component
void APlayerCharacter::MoveRight(float axis)
{
	AddMovementInput(FirstPersonCamera->GetRightVector() * MovementSpeed, axis); // Get the right vector of the first person camera
}

void APlayerCharacter::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu"); // Open the level to return to main menu -- OBSELETE IN VR VERSION
}

// Move the teleport sprite location every frame that the player holds down the teleport button. 
// This allows for the most accurate teleportation placement
void APlayerCharacter::MoveTeleportLocation()
{
	float MaxDistance = 600.0f; // The max distance per teleportation, set upon from the default character movement speed
	FHitResult hit; // Hit Result infromation from line trace will be stored in here
	// Setting up line trace parameters
	FVector startLocation = MC_Left->GetComponentLocation();
	FCollisionQueryParams myParams("TeleportTrace", false, this); // Special parameter for collision to have line trace IGNORE the player otherwise collision errors could occur
	FVector endLocation = startLocation + (MC_Left->GetForwardVector() * MaxDistance);
	// Line trace 
	GetWorld()->LineTraceSingleByChannel(hit, startLocation, endLocation, ECollisionChannel::ECC_Camera, myParams);
	// If an object is hit from the line trace, then set the teleport sprite to its location or just set it to the end trace location
	if (hit.bBlockingHit) SM_TeleportMesh->SetWorldLocation(hit.Location, false);
	else SM_TeleportMesh->SetWorldLocation(endLocation, false);
	// As the level is a flat designed level, simply make sure that the z location of the teleporter is not out of bounds (2 is used to give room for floor placement error)
		if (SM_TeleportMesh->GetComponentLocation().Z > 2.0f)
		{
			bTeleportationIsValid = false; // Less than two is valid, higher is invalid. 
			TeleportInstancedMaterial->SetVectorParameterValue("Colour", FLinearColor::Red); // red teleporter is invalid
		}
		else
		{
			bTeleportationIsValid = true;
			TeleportInstancedMaterial->SetVectorParameterValue("Colour", FLinearColor::Blue); // blue is valid
		}
}

// Teleport the player to the teleporter sprite's location if the location is a valid one
// Sweep the movement so that the player will stop short of any collision that is in the way (stops teleportation through walls etc)
void APlayerCharacter::Teleport()
{
	// Reset teleportation
	if (bTeleportationIsValid) SetActorLocation(FVector(SM_TeleportMesh->GetComponentLocation().X, SM_TeleportMesh->GetComponentLocation().Y, GetActorLocation().Z), true);
	bTeleportationIsValid = false;
	bIsTeleporting = false;
	SM_TeleportMesh->SetWorldLocation(RootComponent->GetComponentLocation());
	SM_TeleportMesh->SetVisibility(false, true); // Hide sprite until it needs to be used again
}

// Activate the teleportation locomation method by making the sprite visible and resetting the variables to defualt
void APlayerCharacter::ActivateTeleportationMethod()
{
	bTeleportationIsValid = false;
	bIsTeleporting = true;
	SM_TeleportMesh->SetVisibility(true, true);
	MoveTeleportLocation();
}

// Interact with the last interactable item the player saw IF they are still looking at it
void APlayerCharacter::Interact()
{
	if (InteractableItem != nullptr) // Make sure that an object isn't NULL before attempting to use it -- would crash otherwise
	{
		InteractableItem->OnInteract(this);
	}
}

// Checks to see whether the item is currently in the players inventory.
// Returns a simple true or false to flag whether the play has it or not
bool APlayerCharacter::CheckInventory(AActor * item)
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == item)
		{
			RemoveItemInInventoryAtIndex(i);
			return true; // item was found in the inventory
		}
	}
	return false; // If the loop completes then the item is not in the inventory
}

// Adds the reference to an item into the player's inventory
void APlayerCharacter::AddItemToIventory(AActor * item)
{
	Inventory.Add(item);
}

// Removes the item from inventory by checking for its reference
void APlayerCharacter::RemoveItemInInvetory(AActor * item)
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		Inventory[i]->Destroy();
		Inventory.RemoveAt(i);
	}
}

// Removes item from inventory and world using an index location instead
void APlayerCharacter::RemoveItemInInventoryAtIndex(int Index)
{
	if (Index < Inventory.Num())
	{
		Inventory[Index]->Destroy();	// Make sure the index is legitimate before use
		Inventory.RemoveAt(Index);
	}
}

// Store a message sent from an object to the player
void APlayerCharacter::SendMessageToPlayer(FString Text)
{
	lastMessage = Text;
	fTextTimer = MaxTextTimer;
	SpawnMessageComponent->SpawnPlayerMessage(FirstPersonCamera->GetComponentLocation(), FirstPersonCamera->GetForwardVector());
}

// Gets the last message the player received from an object
FString APlayerCharacter::GetPlayerMessage()
{
	return lastMessage;
}

// If the player's hand overlaps with an interactable object, the data is stored and visible feedback is given
void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		AInteractable_Maser* overlappedInteractableActor = Cast<AInteractable_Maser>(OtherActor); // Create new pointer that can be used as highest level interactable item
		if (overlappedInteractableActor)
		{
			if (InteractableItem != nullptr) InteractableItem->UnHoveredOver(); // If a new interactable item was hit, unhighlight the last object and swap the reference to the new one
			InteractableItem = overlappedInteractableActor;
			InteractableItem->HoveredOver(); // Highlight the new object
		}
	}
}

// Visual feedback on previous interactable is erased if the object being unoverlapped with is the interactable item stored
void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == InteractableItem)
	{
		if (InteractableItem != nullptr) InteractableItem->UnHoveredOver(); // If a new interactable item was hit, unhighlight the last object and swap the reference to the new one
		InteractableItem = nullptr;
	}
}

// Return the hand component incase it needs to be used
UStaticMeshComponent * APlayerCharacter::GetPlayerHand()
{
	return SM_Hand_R;
}
