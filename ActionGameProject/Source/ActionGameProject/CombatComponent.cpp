// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"

#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "CombatAnimInstance.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupCombatAnimInstance();
	SetupInputComponent();
	ResetAttacks();

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::SetupCombatAnimInstance()
{
	CombatAnimInstance = Cast<UCombatAnimInstance>(GetOwner()->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());

	if (CombatAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatAnimInstance found on %s"), *(GetOwner()->GetName()))
	}
}

void UCombatComponent::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent found on %s"), *(GetOwner()->GetName()))
			
		InputComponent->BindAction("LightAttack", IE_Pressed, this, &UCombatComponent::LightAttack);
		InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &UCombatComponent::HeavyAttack);
	}

}

void UCombatComponent::LightAttack()
{
	if (bReadyForAtkInput)
	{
		if (bChain)
		{
			AttackCount++;
		}

		if (NextLightAttack)
		{
			PendingAttack = NextLightAttack;
			bReadyForAtkInput = false;
			UE_LOG(LogTemp, Warning, TEXT("LightAttack Pressed"));
			if (bReadyToAttack)
			{
				UE_LOG(LogTemp, Warning, TEXT("Executing LightAttack because ReadyToAttack"));
				ExecuteAttack();
			}
		}

		bChain = false;

		
		//GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UCombatComponent::OpenChainWindow, 1.f, false, 1.f);
	}
}

void UCombatComponent::HeavyAttack()
{
	if (bReadyForAtkInput)
	{
		if (bChain)
		{
			AttackCount++;
		}

		if (NextHeavyAttack)
		{
			PendingAttack = NextHeavyAttack;
			bReadyForAtkInput = false;
			UE_LOG(LogTemp, Warning, TEXT("HeavyAttack Pressed"))
			if (bReadyToAttack)
			{
				UE_LOG(LogTemp, Warning, TEXT("Executing HeavyAttack because ReadyToAttack"))
				ExecuteAttack();
			}
		}
			
		bChain = false;

		//GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UCombatComponent::OpenChainWindow, 1.f, false, 2.f);
	}
}

void UCombatComponent::OpenChainWindow()
{
	/*UE_LOG(LogTemp, Warning, TEXT("Chain Window opened"))

	bChain = true;
	bReadyToAttack = true;

	GetWorld()->GetTimerManager().ClearTimer(ChainTimer);
	//GetWorld()->GetTimerManager().SetTimer(ChainTimer, this, &UCombatComponent::CloseChainWindow, 1.f, false, 2.f);*/
}

void UCombatComponent::CloseChainWindow()
{
	/*UE_LOG(LogTemp, Warning, TEXT("Chain Window closed"))

	bChain = false;
	GetWorld()->GetTimerManager().ClearTimer(ChainTimer);
	ResetAttacks();*/
}

void UCombatComponent::HandleReadyToAttack(bool i_bReadyToAttack)
{	
	bReadyToAttack = i_bReadyToAttack;	
	if (bReadyToAttack && PendingAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executing Attack because PendingAttack"));
		ExecuteAttack();
	}
}

void UCombatComponent::SetReadyForAtkInput(bool i_bReadyForAtkInput)	
{	
	bReadyForAtkInput = i_bReadyForAtkInput;	
}

void UCombatComponent::ReadyNextAttacks()
{
	if (CurrentAttack)
	{
		NextLightAttack = &AttackArray[CurrentAttack->NextLightAttackIndex];
		NextHeavyAttack = &AttackArray[CurrentAttack->NextHeavyAttackIndex];
	}
}

void UCombatComponent::ResetAttacks()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Reset"))

	if (&AttackArray[FirstLightAttack])
	{
		NextLightAttack = &AttackArray[FirstLightAttack];
	}

	if (&AttackArray[FirstHeavyAttack])
	{
		NextHeavyAttack = &AttackArray[FirstHeavyAttack];
	}

	CurrentAttack = nullptr;

	AttackCount = 0;
	bReadyToAttack = true;
	bReadyForAtkInput = true;
	PendingAttack = nullptr;
	CombatAnimInstance->SetAttacking(false);
}

void UCombatComponent::ExecuteAttack()
{
	check(PendingAttack != nullptr && "PendingAttack was null");

	CurrentAttack = PendingAttack;
	CombatAnimInstance->SetAttacking(true);
	CombatAnimInstance->SetAttackAnim(CurrentAttack->AttackAnim);

	///TODO Put montage play in the right place
	///TODO Change montage play rate
	CombatAnimInstance->Montage_Play(CurrentAttack->AttackAnim, 0.5f, EMontagePlayReturnType::MontageLength, 0.f, true);
	bReadyToAttack = false;
	PendingAttack = nullptr;

	ReadyNextAttacks();
}

