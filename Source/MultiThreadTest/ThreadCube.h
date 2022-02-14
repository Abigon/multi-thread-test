// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiThreadTestGameModeBase.h"
#include "ThreadCube.generated.h"

UCLASS(BlueprintType)
class MULTITHREADTEST_API AThreadCube : public AActor
{
	GENERATED_BODY()
	
public:	
	AThreadCube();

	void Init(FCubeInfo CubeInfo);

	UFUNCTION(BlueprintNativeEvent)
	void InitBP(FCubeInfo CubeInfo);

protected:
	virtual void BeginPlay() override;

};
