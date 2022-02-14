// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "SimpleMutex_Runnable.h"
#include "SimpleCollectable_Runnable.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "IMessageBus.h"

#include "HAL/ThreadingBase.h"

#include "MultiThreadTestGameModeBase.generated.h"



USTRUCT(BlueprintType)
struct FCubeInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Age = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::Black;
};

USTRUCT()
struct FBusStructMessage
{
	GENERATED_USTRUCT_BODY()
	
	bool bIsColor = false;
	float Age = 0;
	FLinearColor Color = FLinearColor::Black;
	FBusStructMessage(bool InBool = false, float InFloat = 0, FLinearColor InColor = FLinearColor::Black) : bIsColor(InBool), Age(InFloat), Color(InColor) {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateByGeneratorSignature, bool, bIsColor, FLinearColor, NewColor, float, NewAge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateByCubeThreadSignature, FCubeInfo, NewCubeInfo);


UCLASS()
class MULTITHREADTEST_API AMultiThreadTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cube")
	TSubclassOf<class AThreadCube> ThreadCubeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cube")
	float CubeAgeMin = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cube")
	float CubeAgeMax = 40.f;


	UPROPERTY(BlueprintAssignable)
	FOnUpdateByGeneratorSignature OnUpdateByGenerator;
	UPROPERTY(BlueprintAssignable)
	FOnUpdateByCubeThreadSignature OnUpdateByCubeThread;

	TArray<FRunnableThread*> CurrentRunningThread_SimpleMutex;
	FRunnableThread* CurrentRunningThread_SimpleCollectable = nullptr;

	TArray<float> CubeAges;
	FCriticalSection CubeAgesMutex;

	TQueue<FLinearColor, EQueueMode::Mpsc> CubeColors;
	TArray<FLinearColor> CurrentCubeColors;

	TArray<FCubeInfo> CubeInfos;
	FCriticalSection CubeInfoMutex;

	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> ReciveEndpoint_Generator;
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> ReciveEndpoint_CubeInfo;

	void BusMessageHandler_Genetator(const struct FBusStructMessage& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);
	void BusMessageHandler_CubeInfo(const struct FCubeInfo& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	void CreateSimpleMutexThread();
	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	void CreateSimpleCollectableThread();
	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	void StopSimpleMutexThreads();
	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	TArray<FLinearColor > GetCubeColors();
	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	TArray<float> GetCubeAges();
	UFUNCTION(BlueprintCallable, Category = "ThreadTest")
	TArray<FCubeInfo> GetCubesInfo();

	void EventMessage_Generator(bool bIsColor, float Age, FLinearColor Color);
	void EventMessage_CubeInfo(FCubeInfo CubeData);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DestroyAllCubes();

};
