// Fill out your copyright notice in the Description page of Project Settings.

#include "ReadyToAttackNotify.h"

#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

void UReadyToAttackNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UCombatComponent* CombatComponent = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->HandleReadyToAttack(bReadyToAttack);
	}
}
