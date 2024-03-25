// Copyright Epic Games, Inc. All Rights Reserved.

#include "LearnProjectGameMode.h"
#include "LearnProject/Public/Characters/LearnProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALearnProjectGameMode::ALearnProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
