// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CombatAnimInstance.generated.h"


class UAnimSequence;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class ACTIONGAMEPROJECT_API UCombatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetAttackAnim(UAnimMontage* i_AttackAnim);
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetAttackAnim();

	UFUNCTION(BlueprintCallable)
	void SetAttacking(bool i_bAttacking);
	UFUNCTION(BlueprintCallable)
	bool GetAttacking();

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool m_bAttacking = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UAnimMontage* m_AttackAnim = nullptr;

private:
	
};
