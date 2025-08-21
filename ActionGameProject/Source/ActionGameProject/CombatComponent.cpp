// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"

#include "Controllers/ActionGamePlayerController.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "CombatAnimInstance.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Input/AttackInputDirectionEvaluator.h"
#include "Input/InputEvaluator.h"
#include "TimerManager.h"

static const int32 MaxComboStringTextLength = 30;

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	m_ComboStringAsText.Reserve(MaxComboStringTextLength);

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupCombatAnimInstance();
	SetupInputComponent();
	ResetAttacks();

	APawn* pawnOwner = CastChecked<APawn>(GetOwner());
	AActionGamePlayerController* playerController = CastChecked<AActionGamePlayerController>(pawnOwner->Controller);	//SHMANE TODO find a better way than casting
	m_pInputEvaluator = new AttackInputDirectionEvaluator();	//SHMANE TODO remove dependency on implementation class
	m_pInputEvaluator->SetInputBuffer(playerController->GetInputBuffer());

}

void UCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_pInputEvaluator)
	{
		delete m_pInputEvaluator;
		m_pInputEvaluator = nullptr;
	}

	Super::EndPlay(EndPlayReason);
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
	AttackInputDirectionEvaluator* attackInputDirEvaluator = static_cast<AttackInputDirectionEvaluator*>(m_pInputEvaluator);
	if (attackInputDirEvaluator)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDirectionName"), true);
		//if (!EnumPtr) return NSLOCTEXT("Invalid", "Invalid", "Invalid");

		EDirectionName attackDirection = attackInputDirEvaluator->GetAttackInputDirection(GetOwner()->GetActorTransform());
		//EnumPtr->GetDisplayNameText(attackDirection);
		if (EnumPtr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Attack Direction: %s"), *(EnumPtr->GetDisplayValueAsText(attackDirection).ToString())));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Attack Direction: %i"), attackDirection));
		}
	}
	if (bReadyForAtkInput)
	{
		m_ComboStringAsText += "Punch-";
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
		m_ComboStringAsText += "Kick-";
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

void UCombatComponent::LoadPauseAttacks()
{
	if (CurrentAttack)
	{
		if (CurrentAttack->NextPauseLightAttackIndex >= 0)
		{
			NextLightAttack = &AttackArray[CurrentAttack->NextPauseLightAttackIndex];
		}
		if (CurrentAttack->NextPauseHeavyAttackIndex >= 0)
		{
			NextHeavyAttack = &AttackArray[CurrentAttack->NextPauseHeavyAttackIndex];
		}
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

	m_ComboStringAsText.Reset();
	AttackCount = 0;
	bReadyToAttack = true;
	bReadyForAtkInput = true;
	PendingAttack = nullptr;
	CombatAnimInstance->SetAttacking(false);
}

//void UCombatComponent::GetCurrentAttackNotifies(TArray<FAnimNotifyEventReference>& OutActiveNotifies)
//{
//	CurrentAttack->AttackAnim->GetAnimNotifies(0.f, 1.f, false, OutActiveNotifies);
//}

void UCombatComponent::ExecuteAttack()
{
	check(PendingAttack != nullptr && "PendingAttack was null");

	CurrentAttack = PendingAttack;
	CombatAnimInstance->SetAttacking(true);
	CombatAnimInstance->SetAttackAnim(CurrentAttack->AttackAnim);

	///TODO Put montage play in the right place
	///TODO Change montage play rate
	CombatAnimInstance->Montage_Play(CurrentAttack->AttackAnim, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
	bReadyToAttack = false;
	PendingAttack = nullptr;

	ReadyNextAttacks();
}

