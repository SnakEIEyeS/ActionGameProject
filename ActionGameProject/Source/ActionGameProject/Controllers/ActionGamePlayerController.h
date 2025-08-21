// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ActionGamePlayerController.generated.h"

class AInputBuffer;

/**
 * 
 */
UCLASS()
class ACTIONGAMEPROJECT_API AActionGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AActionGamePlayerController();

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void Destroyed() override;

	FORCEINLINE AInputBuffer* GetInputBuffer() const { return m_pInputBuffer; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InputBuffer, meta = (AllowPrivateAccess = "true"))
	AInputBuffer* m_pInputBuffer = nullptr;
	
	
};
