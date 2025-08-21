// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InputEvaluator.generated.h"

//SHMANE TODO consider putting these direction structs and enum in a separate header
UENUM(BlueprintType)
enum class EDirectionInputType : uint8
{
	kDIT_Invalid	UMETA(DisplayName = "Invalid"),
	kDIT_Rising		UMETA(DisplayName = "Rising"),
	kDIT_Falling	UMETA(DisplayName = "Falling"),
	kDIT_Held		UMETA(DisplayName = "Held"),
	kDIT_numTypes	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDirectionName : uint8	//SHMANE TODO rename coz it can be confused with the 'Name' type
{
	kDN_None				UMETA(DisplayName = "None"),
	kDN_Forward				UMETA(DisplayName = "Forward"),
	kDN_Backward			UMETA(DisplayName = "Backward"),
	kDN_Left				UMETA(DisplayName = "Left"),
	kDN_Right				UMETA(DisplayName = "Right"),
	kDN_ForwardToBackward	UMETA(DisplayName = "ForwardToBackward"),
	kDN_BackwardToForward	UMETA(DisplayName = "BackwardToForward"),
	kDN_LeftToRight			UMETA(DisplayName = "LeftToRight"),
	kDN_RightToLeft			UMETA(DisplayName = "RightToLeft"),
	kDN_numTypes			UMETA(Hidden)
};

USTRUCT()
struct FDirectionInputState
{
	GENERATED_BODY()

	FDirectionInputState() {}

	FVector m_startPosition;
	FVector m_endPosition;
	float m_duration;
	EDirectionInputType m_type;
};

class AInputBuffer;

/**
 * 
 */
class ACTIONGAMEPROJECT_API InputEvaluatorBase
{
public:
	InputEvaluatorBase();
	virtual ~InputEvaluatorBase();

	FORCEINLINE void SetInputBuffer(AInputBuffer* i_pInputBuffer) { m_pInputBuffer = i_pInputBuffer; }

protected:
	virtual void EvaluateInputBuffer() = 0;

	//SHMANE TODO is this garbage collected?
	AInputBuffer* m_pInputBuffer = nullptr;
};
