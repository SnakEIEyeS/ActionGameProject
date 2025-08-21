// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGamePlayerController.h"

#include "../Input/AttackInputDirectionEvaluator.h"
#include "../Input/InputBuffer.h"

AActionGamePlayerController::AActionGamePlayerController()
{
}

void AActionGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);

	FActorSpawnParameters inputBufferSpawnParams;
	inputBufferSpawnParams.Name = TEXT("InputBuffer");	//SHMANE TODO improve name
	inputBufferSpawnParams.Owner = this;
	m_pInputBuffer = GetWorld()->SpawnActor<AInputBuffer>(inputBufferSpawnParams);
	m_pInputBuffer->SetPlayerController(this);

}

void AActionGamePlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	//UE_LOG(LogTemp, Warning, TEXT("AActionGamePlayerController::TickActor()"));
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	m_pInputBuffer->Tick(DeltaTime);
}

void AActionGamePlayerController::Destroyed()
{
	m_pInputBuffer = nullptr;

	Super::Destroyed();
}
