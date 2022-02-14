// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleMutex_Runnable.h"
#include "MultiThreadTestGameModeBase.h"
#include <random>

FSimpleMutex_Runnable::FSimpleMutex_Runnable(AMultiThreadTestGameModeBase* OwnerGameMode, bool bIsColorMode, float AgeMin, float AgeMax)
{
	GameModeRef = OwnerGameMode;
	bIsGenerateColor = bIsColorMode;
	AgeGenerateMin = AgeMin;
	AgeGenerateMax = AgeMax;

	SenderGeneratorEndpoint = FMessageEndpoint::Builder("Generator_Sender").Build();
}

FSimpleMutex_Runnable::~FSimpleMutex_Runnable()
{
}

bool FSimpleMutex_Runnable::Init()
{
	return true;
}

uint32 FSimpleMutex_Runnable::Run()
{
	while (!bIsStopGenerate)
	{
		FLinearColor Color = FLinearColor::Black;
		float Age = 0.f;
		if (bIsGenerateColor)
		{
			float R = GetRandomInt(0, 255) / 1000.f;
			float G = GetRandomInt(0, 255) / 1000.f;
			float B = GetRandomInt(0, 255) / 1000.f;
			Color = FLinearColor::FLinearColor(R, G, B, 1.f);
			GameModeRef->CubeColors.Enqueue(Color);
		}
		else
		{
			Age = GetRandomInt(AgeGenerateMin, AgeGenerateMax);
			GameModeRef->CubeAgesMutex.Lock();
			GameModeRef->CubeAges.Add(Age);
			GameModeRef->CubeAgesMutex.Unlock();
		}
		
		if (SenderGeneratorEndpoint.IsValid())
		{
			SenderGeneratorEndpoint->Publish<FBusStructMessage>(new FBusStructMessage(bIsGenerateColor, Age, Color));
		}
		FPlatformProcess::Sleep(0.5f);
	}

	return 1;
}

void FSimpleMutex_Runnable::Stop()
{
	bIsStopGenerate = true;
}

void FSimpleMutex_Runnable::Exit()
{
	if (SenderGeneratorEndpoint.IsValid())
	{
		SenderGeneratorEndpoint.Reset();
	}
	GameModeRef = nullptr;
}

int FSimpleMutex_Runnable::GetRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(min, max);
	return distr(gen);
}
