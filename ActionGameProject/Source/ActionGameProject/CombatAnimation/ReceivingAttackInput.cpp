// Fill out your copyright notice in the Description page of Project Settings.

#include "ReceivingAttackInput.h"

#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

void UReceivingAttackInput::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UCombatComponent* CombatComponent = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetReadyForAtkInput(bReceivingAttackInput);
	}
}
