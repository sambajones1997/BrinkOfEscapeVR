#include "PlayerMessageText.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "ConstructorHelpers.h"

// Sets default values
APlayerMessageText::APlayerMessageText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render Component")); // Create the text render component
	TextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TextRenderComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform); // Attach the Text Render to the root component with no offset
	TextRenderComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center); // Align the text to the centre
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> TextMat(TEXT("/Engine/EngineMaterials/DefaultTextMaterialTranslucent.DefaultTextMaterialTranslucent")); // Translucent mat to see through walls
	if (TextMat.Object)TextRenderComponent->SetMaterial(0, TextMat.Object);
}

// Called when the game starts or when spawned
void APlayerMessageText::BeginPlay()
{
	Super::BeginPlay();
	TextRenderComponent->SetText(*GetPlayerMessage()); // Set text of the text render component to the last message
	SetLifeSpan(5.0f); // After 5 seconds, the text will be destroyed
}

// Called every frame
void APlayerMessageText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FacePlayer(); // face the player each frame - not intensive
}

// Get the last message that the player received
FString APlayerMessageText::GetPlayerMessage()
{
	APlayerCharacter* playerCharacter; // Player Character pointer is cast to useing the current world and player index of 0 (first player)
	playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	return playerCharacter->GetPlayerMessage(); // Return that message
}

// Have the message follow the player so wherever they move, they can read the text
void APlayerMessageText::FacePlayer()
{
	// Get the rotation of the first player's camera in the current world by using the vector between the text location and the camera
	FRotator Rot = FRotationMatrix::MakeFromX(UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation() - GetActorLocation()).Rotator();
	SetActorRotation(Rot); // Set the rotation of this actor to face the player
}

