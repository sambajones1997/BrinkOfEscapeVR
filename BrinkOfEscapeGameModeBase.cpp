// Fill out your copyright notice in the Description page of Project Settings.

#include "BrinkOfEscapeGameModeBase.h"
#include "PlayerCharacter.h"

ABrinkOfEscapeGameModeBase::ABrinkOfEscapeGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass(); // Set the default pawn class of the game mode to the player character
}


