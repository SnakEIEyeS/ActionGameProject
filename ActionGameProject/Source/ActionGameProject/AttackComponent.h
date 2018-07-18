// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class UInputComponent;
//struct FTimerHandle;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	UInputComponent* InputComponent = nullptr;
	FTimerHandle ChainTimer;
	uint32 AttackCount = 1;
	bool bChain = false;	//Probably won't need bChain once animations trigger event to load next attack or to reset to 1st attack
	bool bReadyToAttack = true;

	void SetupInputComponent();

	void LightAttack();
	void HeavyAttack();

	void OpenChainWindow();
	void CloseChainWindow();

	void ReadyNextAttack();
	void ResetAttack();

		
	
};
