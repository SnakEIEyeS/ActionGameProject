// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UAnimMontage;
class UAnimSequence;
class UCombatAnimInstance;
class UInputComponent;
//struct FTimerHandle;


USTRUCT(BlueprintType)
struct FAttackNode
{
	GENERATED_BODY()

		//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//UAnimSequence* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextLightAttackIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextHeavyAttackIndex = 0;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAMEPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OpenChainWindow();
	void CloseChainWindow();

private:
	UPROPERTY(EditAnywhere)
	TArray<FAttackNode> AttackArray;

	//UPROPERTY(VisibleInstanceOnly)
	FAttackNode* CurrentAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextLightAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextHeavyAttack = nullptr;

	UCombatAnimInstance* CombatAnimInstance = nullptr;
	UInputComponent* InputComponent = nullptr;
	FTimerHandle ChainTimer;
	uint32 AttackCount = 1;

	const uint8 FirstLightAttack = 0;
	const uint8 FirstHeavyAttack = 1;

	bool bChain = false;	//Probably won't need bChain once animations trigger event to load next attack or to reset to 1st attack
	bool bReadyToAttack = true;

	void SetupCombatAnimInstance();
	void SetupInputComponent();

	void LightAttack();
	void HeavyAttack();

	void ReadyNextLightAttack();
	void ReadyNextHeavyAttack();
	void ResetAttacks();



};
