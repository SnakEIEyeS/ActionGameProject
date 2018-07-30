// Fill out your copyright notice in the Description page of Project Settings.

#include "CloseChainWindowNotify.h"

#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

void UCloseChainWindowNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UCombatComponent* CombatComponent = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->CloseChainWindow();
	}
}
