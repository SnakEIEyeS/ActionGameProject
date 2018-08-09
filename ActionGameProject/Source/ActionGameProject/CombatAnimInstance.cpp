// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatAnimInstance.h"

#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"


void UCombatAnimInstance::SetAttackAnim(UAnimMontage* i_AttackAnim)
{
	m_AttackAnim = i_AttackAnim;

	UE_LOG(LogTemp, Warning, TEXT("SetAttackAnim got called!"))
}

UAnimMontage* UCombatAnimInstance::GetAttackAnim()
{
	return m_AttackAnim;
}

void UCombatAnimInstance::SetAttacking(bool i_bAttacking)
{
	m_bAttacking = i_bAttacking;
}

bool UCombatAnimInstance::GetAttacking()
{
	return m_bAttacking;
}

