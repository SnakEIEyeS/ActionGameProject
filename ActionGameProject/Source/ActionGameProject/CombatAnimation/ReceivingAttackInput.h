// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ReceivingAttackInput.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAMEPROJECT_API UReceivingAttackInput : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReceivingAttackInput = false;
	
	
};
