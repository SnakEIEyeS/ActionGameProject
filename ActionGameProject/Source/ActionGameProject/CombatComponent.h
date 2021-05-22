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

//struct FAttackNode;
USTRUCT(BlueprintType)
struct FAttackNode
{
	GENERATED_BODY()

		//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//UAnimSequence* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextLightAttackIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextHeavyAttackIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextPauseLightAttackIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NextPauseHeavyAttackIndex = -1;
};


//TODO: separate input from CombatComp so that this can be used on AI but different input types (Player or AI) can call funcs on this 
//in same style but based on different processing
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Attacks")
	uint8 FirstLightAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Attacks")
	uint8 FirstHeavyAttack = 1;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OpenChainWindow();
	void CloseChainWindow();
	void HandleReadyToAttack(bool i_bReadyToAttack);
	void SetReadyForAtkInput(bool i_bReadyForAtkInput);

	void LoadPauseAttacks();
	void ResetAttacks();

	UFUNCTION(BlueprintCallable)
	inline UAnimMontage* GetCurrentAttackMontage() { return CurrentAttack->AttackAnim; }

	UFUNCTION(BlueprintCallable)
	inline bool IsReadyForAttackInput() { return bReadyForAtkInput; }

	UFUNCTION(BlueprintCallable)
	inline bool IsAttackPending() { return PendingAttack ? true : false; }
	//UFUNCTION(BlueprintCallable)
	//void GetCurrentAttackNotifies(TArray<FAnimNotifyEventReference>& OutActiveNotifies);

	UFUNCTION(BlueprintCallable)
	inline FString GetComboStringText() { return m_ComboStringAsText; }

private:
	UPROPERTY(EditAnywhere)
	TArray<FAttackNode> AttackArray;

	//UPROPERTY(VisibleInstanceOnly)
	FAttackNode* CurrentAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextLightAttack = nullptr;

	//UPROPERTY(VisibleAnywhere)
	FAttackNode* NextHeavyAttack = nullptr;

	FAttackNode* PendingAttack = nullptr;

	UCombatAnimInstance* CombatAnimInstance = nullptr;
	UInputComponent* InputComponent = nullptr;
	FString m_ComboStringAsText;
	FTimerHandle ChainTimer;
	uint32 AttackCount = 1;

	bool bChain = false;	//Probably won't need bChain once animations trigger event to load next attack or to reset to 1st attack
	bool bReadyForAtkInput = true;
	bool bReadyToAttack = true;

	void SetupCombatAnimInstance();
	void SetupInputComponent();

	void LightAttack();
	void HeavyAttack();

	void ExecuteAttack();
	void ReadyNextAttacks();
	

};
