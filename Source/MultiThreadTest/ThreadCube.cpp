// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadCube.h"

AThreadCube::AThreadCube()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AThreadCube::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThreadCube::Init(FCubeInfo CubeInfo)
{
	InitBP(CubeInfo);
}

void AThreadCube::InitBP_Implementation(FCubeInfo CubeInfo)
{

}