// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "IMessageBus.h"



class AMultiThreadTestGameModeBase;


class MULTITHREADTEST_API FSimpleCollectable_Runnable : public FRunnable
{

public:

	FSimpleCollectable_Runnable(AMultiThreadTestGameModeBase* OwnerGameMode);
	~FSimpleCollectable_Runnable();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	AMultiThreadTestGameModeBase* GameModeRef = nullptr;
	FThreadSafeBool bIsStopCollectable = false;

	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> SenderCollectorEndpoint;
};
