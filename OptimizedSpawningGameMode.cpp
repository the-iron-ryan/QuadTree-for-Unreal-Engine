// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OptimizedSpawningGameMode.h"
#include "OptimizedSpawningCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "QTree.h"

AOptimizedSpawningGameMode::AOptimizedSpawningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	QTree tree(FVector2D(0.0f, 0.0f), FVector2D(10.0f, 10.0f));
}
