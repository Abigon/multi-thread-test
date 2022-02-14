// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCollectable_Runnable.h"
#include "MultiThreadTestGameModeBase.h"

FSimpleCollectable_Runnable::FSimpleCollectable_Runnable(AMultiThreadTestGameModeBase* OwnerGameMode)
{
	GameModeRef = OwnerGameMode;

	SenderCollectorEndpoint = FMessageEndpoint::Builder("Collector_Sender").Build();
}

FSimpleCollectable_Runnable::~FSimpleCollectable_Runnable()
{
}

bool FSimpleCollectable_Runnable::Init()
{
	return true;
}

uint32 FSimpleCollectable_Runnable::Run()
{
	int32 i = 0;

	while (!bIsStopCollectable)
	{
		FPlatformProcess::Sleep(0.3f);

		int32 sizeAges = GameModeRef->CubeAges.Num();
		TArray<FLinearColor> AvaibleColors = GameModeRef->GetCubeColors();
		int32 sizeColors = AvaibleColors.Num();
		if ((sizeAges > 0) && (sizeColors > 0))
		{
			FCubeInfo newCube;
			newCube.ID = i;
			i++;

			GameModeRef->CubeAgesMutex.Lock();
			newCube.Age = GameModeRef->CubeAges[0];
			GameModeRef->CubeAges.RemoveAt(0);
			GameModeRef->CubeAgesMutex.Unlock();

			newCube.Color = AvaibleColors[0];
			GameModeRef->CurrentCubeColors.RemoveAt(0);

			{
				FScopeLock CubesScopedLock(&GameModeRef->CubeInfoMutex);
				GameModeRef->CubeInfos.Add(newCube);
			}

			if (SenderCollectorEndpoint.IsValid())
			{
				SenderCollectorEndpoint->Publish<FCubeInfo>(new FCubeInfo(newCube));
			}
		}
	}

	return 1;
}

void FSimpleCollectable_Runnable::Stop()
{
	bIsStopCollectable = true;
}

void FSimpleCollectable_Runnable::Exit()
{
	if (SenderCollectorEndpoint.IsValid())
	{
		SenderCollectorEndpoint.Reset();
	}
	GameModeRef = nullptr;
}
