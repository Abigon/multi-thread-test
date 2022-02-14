// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "IMessageBus.h"

class AMultiThreadTestGameModeBase;

class MULTITHREADTEST_API FSimpleMutex_Runnable : public FRunnable
{

public:

	FSimpleMutex_Runnable(AMultiThreadTestGameModeBase* OwnerGameMode, bool bIsColorMode, float AgeMin, float AgeMax);
	~FSimpleMutex_Runnable();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;


	int GetRandomInt(int min, int max);

	bool bIsGenerateColor = false;
	float AgeGenerateMin = 0.f;
	float AgeGenerateMax = 1.f;

	AMultiThreadTestGameModeBase* GameModeRef = nullptr;
	FThreadSafeBool bIsStopGenerate = false;


	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> SenderGeneratorEndpoint;
};
