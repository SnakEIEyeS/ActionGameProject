// Fill out your copyright notice in the Description page of Project Settings.

#include "ResetAttacksNotify.h"

#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

void UResetAttacksNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UCombatComponent* CombatComponent = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->ResetAttacks();
	}
}


