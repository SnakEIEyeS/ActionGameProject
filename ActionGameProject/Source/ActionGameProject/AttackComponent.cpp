// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackComponent.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"


// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
	
}


// Called every frame
void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttackComponent::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent found on %s"), *(GetOwner()->GetName()))
			
		InputComponent->BindAction("LightAttack", IE_Pressed, this, &UAttackComponent::LightAttack);
		InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &UAttackComponent::HeavyAttack);
	}
	
}

void UAttackComponent::LightAttack()
{
	if (bReadyToAttack)
	{
		if (bChain)
		{
			AttackCount++;
		}
		bChain = false;

		UE_LOG(LogTemp, Warning, TEXT("LightAttack Pressed, AttackCount: %d"), AttackCount);
		GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UAttackComponent::OpenChainWindow, 1.f, false, 1.f);

		bReadyToAttack = false;
	}
}

void UAttackComponent::HeavyAttack()
{
	if (bReadyToAttack)
	{
		if (bChain)
		{
			AttackCount++;
		}
		bChain = false;

		UE_LOG(LogTemp, Warning, TEXT("HeavyAttack Pressed, AttackCount: %d"), AttackCount)
		GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UAttackComponent::OpenChainWindow, 1.f, false, 2.f);

		bReadyToAttack = false;
	}
}

void UAttackComponent::OpenChainWindow()
{
	UE_LOG(LogTemp, Warning, TEXT("Chain Window opened"))

	bChain = true;
	bReadyToAttack = true;

	GetWorld()->GetTimerManager().ClearTimer(ChainTimer);
	GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UAttackComponent::CloseChainWindow, 1.f, false, 2.f);
}

void UAttackComponent::CloseChainWindow()
{
	UE_LOG(LogTemp, Warning, TEXT("Chain Window closed"))

	bChain = false;
	GetWorld()->GetTimerManager().ClearTimer(ChainTimer);
	ResetAttack();
}

void UAttackComponent::ReadyNextAttack()
{
}

void UAttackComponent::ResetAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Reset"))

	AttackCount = 0;
	bReadyToAttack = true;
}

