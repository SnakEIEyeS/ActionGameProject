// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class UAnimSequence;
class UInputComponent;
//struct FTimerHandle;


USTRUCT(BlueprintType)
struct FAttackNode
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextLightAttackIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextHeavyAttackIndex = 0;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAMEPROJECT_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
		TArray<FAttackNode> AttackArray;

	//UPROPERTY(VisibleInstanceOnly)
	FAttackNode* CurrentAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextLightAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextHeavyAttack = nullptr;

	UInputComponent* InputComponent = nullptr;
	FTimerHandle ChainTimer;
	uint32 AttackCount = 1;

	const uint8 FirstLightAttack = 0;
	const uint8 FirstHeavyAttack = 1;

	bool bChain = false;	//Probably won't need bChain once animations trigger event to load next attack or to reset to 1st attack
	bool bReadyToAttack = true;

	void SetupInputComponent();

	void LightAttack();
	void HeavyAttack();

	void OpenChainWindow();
	void CloseChainWindow();

	void ReadyNextLightAttack();
	void ReadyNextHeavyAttack();
	void ResetAttacks();



};
