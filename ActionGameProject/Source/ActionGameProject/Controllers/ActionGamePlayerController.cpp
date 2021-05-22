// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGamePlayerController.h"

#include "../Input/InputBuffer.h"

AActionGamePlayerController::AActionGamePlayerController()
{
}

void AActionGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);

	FActorSpawnParameters inputBufferSpawnParams;
	inputBufferSpawnParams.Name = TEXT("InputBufferFromSpawnActor");
	inputBufferSpawnParams.Owner = this;
	m_pInputBuffer = GetWorld()->SpawnActor<AInputBuffer>(inputBufferSpawnParams);
	m_pInputBuffer->SetPlayerController(this);
}

void AActionGamePlayerController::Destroyed()
{
	m_pInputBuffer = nullptr;

	Super::Destroyed();
}
