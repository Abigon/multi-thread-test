// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiThreadTestGameModeBase.h"
#include "ThreadCube.h"
#include "Kismet/GameplayStatics.h"


void AMultiThreadTestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ReciveEndpoint_Generator = FMessageEndpoint::Builder("Reciever_Generator").Handling<FBusStructMessage>(this, &AMultiThreadTestGameModeBase::BusMessageHandler_Genetator);
	ReciveEndpoint_CubeInfo = FMessageEndpoint::Builder("Reciever_CubeInfo").Handling<FCubeInfo>(this, &AMultiThreadTestGameModeBase::BusMessageHandler_CubeInfo);

	if (ReciveEndpoint_Generator.IsValid())
	{
		ReciveEndpoint_Generator->Subscribe<FBusStructMessage>();
	}
	if (ReciveEndpoint_CubeInfo.IsValid())
	{
		ReciveEndpoint_CubeInfo->Subscribe<FCubeInfo>();
	}
}

void AMultiThreadTestGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSimpleMutexThreads();

	if (ReciveEndpoint_Generator.IsValid())
	{
		ReciveEndpoint_Generator.Reset();
	}
	if (ReciveEndpoint_CubeInfo.IsValid())
	{
		ReciveEndpoint_CubeInfo.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AMultiThreadTestGameModeBase::CreateSimpleMutexThread()
{
	for (int32 i = 0; i < 4; i++)
	{
		bool bFlag = (i%2 == 0);

		FSimpleMutex_Runnable *MySimpleMutex_Runnable = new FSimpleMutex_Runnable(this, bFlag, CubeAgeMin, CubeAgeMax);

		CurrentRunningThread_SimpleMutex.Add(FRunnableThread::Create(MySimpleMutex_Runnable, TEXT("SimpleMutex Thread"), 0, EThreadPriority::TPri_Lowest));
	}
}

void AMultiThreadTestGameModeBase::CreateSimpleCollectableThread()
{
	DestroyAllCubes();

	FSimpleCollectable_Runnable* MySimpleCollectable_Runnable = new FSimpleCollectable_Runnable(this);

	CurrentRunningThread_SimpleCollectable = FRunnableThread::Create(MySimpleCollectable_Runnable, TEXT("SimpleCollectable Thread"), 0, EThreadPriority::TPri_Lowest);
}

void AMultiThreadTestGameModeBase::StopSimpleMutexThreads()
{
	if (CurrentRunningThread_SimpleMutex.Num() > 0)
	{
		for (auto RunnableThread : CurrentRunningThread_SimpleMutex)
		{
			if (RunnableThread)
			{
				RunnableThread->Kill(true);
			}
		}
		CurrentRunningThread_SimpleMutex.Empty();
	}

	if (CurrentRunningThread_SimpleCollectable)
	{
		CurrentRunningThread_SimpleCollectable->Kill(true);
		CurrentRunningThread_SimpleCollectable = nullptr;
	}
}

TArray<FLinearColor> AMultiThreadTestGameModeBase::GetCubeColors()
{
	TArray<FLinearColor> result;
	FLinearColor Color;
	while (CubeColors.Dequeue(Color))
	{
		result.Add(Color);
	}
	CurrentCubeColors.Append(result);

	return CurrentCubeColors;
}

TArray<float> AMultiThreadTestGameModeBase::GetCubeAges()
{
	{
		FScopeLock AgesScopedLock(&CubeAgesMutex);
		return CubeAges;
	}
}

TArray<FCubeInfo> AMultiThreadTestGameModeBase::GetCubesInfo()
{
	return TArray<FCubeInfo>();
}

void AMultiThreadTestGameModeBase::BusMessageHandler_Genetator(const struct FBusStructMessage& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	EventMessage_Generator(Message.bIsColor, Message.Age, Message.Color);
}

void AMultiThreadTestGameModeBase::BusMessageHandler_CubeInfo(const struct FCubeInfo& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	EventMessage_CubeInfo(Message);
}

void AMultiThreadTestGameModeBase::EventMessage_Generator(bool bIsColor, float Age, FLinearColor Color)
{
	OnUpdateByGenerator.Broadcast(bIsColor, Color, Age);
}

void AMultiThreadTestGameModeBase::EventMessage_CubeInfo(FCubeInfo CubeData)
{
	OnUpdateByCubeThread.Broadcast(CubeData);

	const auto World = GetWorld();
	if (World && ThreadCubeClass)
	{
		const int32 row_count = 19;
		int32 z_shift = (CubeData.ID - (CubeData.ID % row_count)) / row_count;
		
		FVector SpawnLoc = FVector(900.f, (100.f * (CubeData.ID % row_count)) - 900.f, 100.f * z_shift + 100.f);
		FRotator SpawnRot = FRotator(0);

		auto myCube = World->SpawnActor<AThreadCube>(ThreadCubeClass, SpawnLoc, SpawnRot, FActorSpawnParameters());
		if (myCube)
		{
			myCube->Init(CubeData);
		}
	}
}

void AMultiThreadTestGameModeBase::DestroyAllCubes()
{
	TArray<AActor*> TempCubes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ThreadCubeClass, TempCubes);

	if (TempCubes.Num() > 0)
	{
		for (auto TempCube : TempCubes)
		{
			if (TempCube)
			{
				TempCube->Destroy();
			}
		}
	}
}