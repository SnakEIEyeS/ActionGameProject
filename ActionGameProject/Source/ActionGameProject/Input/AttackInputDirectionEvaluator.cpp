// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackInputDirectionEvaluator.h"

#include "InputBuffer.h"
#include "Containers/BasicArray.h"
#include "Containers/CircularBuffer.h"

AttackInputDirectionEvaluator::AttackInputDirectionEvaluator()
{
}

AttackInputDirectionEvaluator::~AttackInputDirectionEvaluator()
{
	m_pInputBuffer = nullptr;
}

//SHMANE TODO this should take in an evaluationStartTime or bufferScrubDuration parameter
//SHMANE TODO does not support very fast input, possibly coz the proper edges are not detected in this case
//SHMANE my guess - DMC5 does a subtle trick by having the camera at an angle from the player char's back. This helps because players will in most cases not provide perfectly straight back or forward input.
EDirectionName AttackInputDirectionEvaluator::GetAttackInputDirection(const FTransform& i_targetSpace)
{
	//SHMANE TODO this binary search should be inside InputBuffer. We should be able to request FrameInputData at a specific time
	//Get circ bufffer
	TSharedPtr<TCircularBuffer<FInputData>> frameInputArray = m_pInputBuffer->GetBuffer();

	//use binary search to find the index that has input data for time <= currentTime - x secs
	uint32 lastWriteIndex = m_pInputBuffer->GetLastWriteIndex();
	float evaluationStartTime = (*frameInputArray)[lastWriteIndex].m_timeStamp - m_evaluationTime;

	uint32 lowIndex = 0;
	uint32 evaluationEndIndex = lastWriteIndex;
	uint32 nextWriteIndex = frameInputArray->GetNextIndex(lastWriteIndex);
	if (lastWriteIndex != frameInputArray->Capacity() - 1 && (*frameInputArray)[nextWriteIndex].m_timeStamp != 0.0f)
	{
		if ((*frameInputArray)[frameInputArray->Capacity() - 1].m_timeStamp >= evaluationStartTime)
		{
			lowIndex = nextWriteIndex;
			evaluationEndIndex = frameInputArray->Capacity() - 1;
		}
	}
	
	uint32 evaluationStartIndex = lowIndex;
	uint32 highIndex = evaluationEndIndex;
	while (true)
	{
		if ((*frameInputArray)[lowIndex].m_timeStamp >= evaluationStartTime)
		{
			evaluationStartIndex = FMath::Max(0, static_cast<int32>(lowIndex - 1));
			break;
		}
		if ((*frameInputArray)[highIndex].m_timeStamp <= evaluationStartTime)
		{
			evaluationStartIndex = highIndex;
			break;
		}

		uint32 midIndex = (lowIndex + highIndex) / 2;
		if (FMath::IsNearlyEqual((*frameInputArray)[midIndex].m_timeStamp, evaluationStartTime))
		{
			evaluationStartIndex = midIndex;
			break;
		}
		else
		{
			if ((*frameInputArray)[midIndex].m_timeStamp < evaluationStartTime)
			{
				lowIndex = midIndex + 1;
			}
			else
			{
				highIndex = midIndex - 1;
			}

			if (lowIndex == highIndex)
			{
				evaluationStartIndex = FMath::Max(0, static_cast<int32>(lowIndex - 1));
				break;
			}
		}
	}

	//use bitonic sequence search on StickDistanceSquared to find satisfying input points
	//SHMANE TODO clamp values to 0.2f and 0.8f or the implementation (maybe) fails
	//SHMANE TODO add forgiveness for 1st point found, if a rising/falling edge was made with its previous point outside the search extents. Not if I don't check last element in eval range for forgiveness.
	TBasicArray<FDirectionInputState> directionStates;
	float lowThreshold = 0.4f;	//SHMANE TODO rename. This needs to be a member data
	float highThreshold = 0.65f;	//SHMANE TODO rename. This needs to be a member data
	uint32 lastExtremeIndex = 0;
	auto isChangePoint = [&frameInputArray, lowThreshold, highThreshold](uint32 index) 
	{
		//SHMANE TODO use >=0.8f || <= 0.1f

		FInputData frameInputData = (*frameInputArray)[index];
		if ((FMath::IsNearlyEqual(frameInputData.m_leftThumbstick_Distance, lowThreshold)
			|| FMath::IsNearlyEqual(frameInputData.m_leftThumbstick_Distance, highThreshold))
			&& (*frameInputArray)[index - 1].m_leftThumbstick_Distance != (*frameInputArray)[index + 1].m_leftThumbstick_Distance)
		{
			return true;
		}
		return false;
	};

	auto isHighChangePoint = [highThreshold](float previousStickDistance, float currentStickDistance)
	{
		return currentStickDistance >= highThreshold ? previousStickDistance < highThreshold : previousStickDistance >= highThreshold;
	};

	auto isLowChangePoint = [lowThreshold](float previousStickDistance, float currentStickDistance)
	{
		return currentStickDistance <= lowThreshold ? previousStickDistance > lowThreshold : previousStickDistance <= lowThreshold;
	};

	struct ExtremeChangePoint
	{
		enum class ChangePointType : uint8
		{
			kCPT_None,
			kCPT_Low,
			kCPT_High
		};
		uint32 m_bufferIndex;
		ChangePointType m_changePointType;

		ExtremeChangePoint() : m_bufferIndex(UINT32_MAX), m_changePointType(ChangePointType::kCPT_None)
		{
		}

		ExtremeChangePoint(uint32 i_bufferIndex, ChangePointType i_changePointType) : m_bufferIndex(i_bufferIndex), m_changePointType(i_changePointType)
		{
		}
	};

	/*if (FMath::IsNearlyEqual((*frameInputArray)[evaluationStartIndex].m_leftThumbstick_Distance, lowThreshold)
		|| FMath::IsNearlyEqual((*frameInputArray)[evaluationStartIndex].m_leftThumbstick_Distance, highThreshold))
	{
		lastExtremeIndex = evaluationStartIndex;
	}*/
	//SHMANE TODO this can be optimized if we go from end to start index, stop if we add 8 direction states
	//reduces the chances of reallocation of array - already reserves 16 though which might be more than enough
	//If going in reverse here, then I don't need to run through the full range. Early out when direction can be finalized
	float cosMaxHoldAngleDifference = FMath::Cos(FMath::DegreesToRadians(m_maxHoldAngleDifference));
	ExtremeChangePoint currentChangePoint;
	ExtremeChangePoint lastChangePoint;
	for (uint32 currentIndex = evaluationStartIndex + 1; currentIndex <= evaluationEndIndex; ++currentIndex)
	{
		bool bChangePointDetected = false;
		if (isHighChangePoint((*frameInputArray)[currentIndex - 1].m_leftThumbstick_Distance, (*frameInputArray)[currentIndex].m_leftThumbstick_Distance))
		{
			bChangePointDetected = true;
			currentChangePoint = ExtremeChangePoint(currentIndex, ExtremeChangePoint::ChangePointType::kCPT_High);
		}
		else if (isLowChangePoint((*frameInputArray)[currentIndex - 1].m_leftThumbstick_Distance, (*frameInputArray)[currentIndex].m_leftThumbstick_Distance))
		{
			bChangePointDetected = true;
			currentChangePoint = ExtremeChangePoint(currentIndex, ExtremeChangePoint::ChangePointType::kCPT_Low);
		}

		if (bChangePointDetected)
		{
			if (lastChangePoint.m_changePointType != ExtremeChangePoint::ChangePointType::kCPT_None)
			{
				const FInputData currentInputData = (*frameInputArray)[currentChangePoint.m_bufferIndex];
				const FInputData previousInputData = (*frameInputArray)[lastChangePoint.m_bufferIndex];

				FDirectionInputState directionInputState;
				directionInputState.m_duration = currentInputData.m_timeStamp - previousInputData.m_timeStamp;
				directionInputState.m_startPosition = FVector(previousInputData.m_leftThumbstick_X.X, previousInputData.m_leftThumbstick_Y.X, 0.f);
				directionInputState.m_endPosition = FVector(currentInputData.m_leftThumbstick_X.X, currentInputData.m_leftThumbstick_Y.X, 0.f);
				
				//check which of rising/falling, sustain edge
				//SHMANE TODO can't use this anymore, check if 1 is lower and other is higher (maybe compare with 0.5). Or might have to store info about 'ChangePoint' being hi or lo
				//if (!FMath::IsNearlyEqual(previousInputData.m_leftThumbstick_Distance, currentInputData.m_leftThumbstick_Distance))
				if(currentChangePoint.m_changePointType != lastChangePoint.m_changePointType)
				{
					//add a rising/falling edge
					if (directionInputState.m_duration < m_maxTimeToReachExtremes)
					{
						//directionInputState.m_type = previousInputData.m_leftThumbstick_Distance < currentInputData.m_leftThumbstick_Distance ? 
						directionInputState.m_type = currentChangePoint.m_changePointType == ExtremeChangePoint::ChangePointType::kCPT_High ? 
							EDirectionInputType::kDIT_Rising : EDirectionInputType::kDIT_Falling;
					}
					else
					{
						directionInputState.m_type = EDirectionInputType::kDIT_Invalid;
					}
				}
				else
				{
					//add a sustain edge
					//if (currentInputData.m_leftThumbstick_Distance >= highThreshold 
					if (currentChangePoint.m_changePointType == ExtremeChangePoint::ChangePointType::kCPT_High 
						&& FVector::DotProduct((directionInputState.m_startPosition * 5.0f).GetUnsafeNormal(), (directionInputState.m_endPosition * 5.0f).GetUnsafeNormal()) < cosMaxHoldAngleDifference)
					{
						//SHMANE TODO this is allowing quarter turns to be detected as cardinal directions because the last element in the buffer range is not being checked. So if the stick is not let go, it can still lead to the last valid direction drawn being the result.
						directionInputState.m_type = EDirectionInputType::kDIT_Invalid;
					}
					else
					{
						directionInputState.m_type = EDirectionInputType::kDIT_Held;
					}

					//many things could happen in a sustain edge like wonky input, rising/falling input that doesn't reach thresholds.
					//in that case the sustain edge would have a long duration, thus invalidating the previous edge from being attributed to an attack - for this evaluator
					
				}
				//add to array of edges
				directionStates.Emplace(directionInputState);	//SHMANE TODO this uses Forward<>(). Should I use std::move? Study std::forward for sure

				//SHMANE TODO account for quarter-turn edges - just for fun. It doesn't make sense in this implementation... 
				//because we check directions relative to character and not camera/stick-space. Tough for players to do a quarter-turn 'accurately'
			}
			//lastExtremeIndex = currentIndex;
			lastChangePoint = currentChangePoint;
		}
	}
	//SHMANE TODO check last point in array extents to handle the case where stick is pulled and just held forever
	//OR this could be forgiveness mechanic since checked buffer is 1 sec. Test to see if good
	// 09/22/21 Not checking the last one is leading to very fast input not giving direction

	if (directionStates.GetData() == nullptr)
	{
		return EDirectionName::kDN_None;
	}

	//SHMANE TODO add logs for WHY direction to return is evaluated as None
	EDirectionName determinedAttackDirection = EDirectionName::kDN_None;
	FMatrix worldToTarget = i_targetSpace.ToMatrixNoScale().InverseFast();
	float successiveHeldTime = 0.0f;
	bool bDirectionFinalized = false;
	float cosMaxDirectionAngleError = FMath::Cos(FMath::DegreesToRadians(m_maxDirectionAngleError));
	for (int32 i = directionStates.Num() - 1; i >= 0 && !bDirectionFinalized; --i)	//index as signed int because it needs to or can go below 0
	{
		FDirectionInputState directionState(directionStates[i]);	//SHMANE TODO make it a pointer
		switch (directionState.m_type)
		{
		case EDirectionInputType::kDIT_Invalid:
		{
			bDirectionFinalized = true;
			break;
		}

		case EDirectionInputType::kDIT_Held:
		{
			successiveHeldTime += directionState.m_duration;
			if (successiveHeldTime > m_longHoldThreshold)
			{
				//bDirectionFinalized = true;
			}
			break;
		}

		case EDirectionInputType::kDIT_Rising:
		{
			successiveHeldTime = 0.0f;

			FVector direction_stickSpace = directionState.m_endPosition - directionState.m_startPosition;
			direction_stickSpace.Normalize();
			//FVector direction_targetSpace = worldToTarget.InverseTransformVector(direction_stickSpace);
			FVector direction_targetSpace = direction_stickSpace;
			//direction_targetSpace.Normalize();	//should be normalized in previous step

			EDirectionName currentDirection = EDirectionName::kDN_None;
			//Stick on vertical, i.e. backward or forward is (0, [-1 to 1], 0). So dot with (0, 1, 0) to get forward or backward
			float dotDirectionForward = FVector::DotProduct(direction_targetSpace, FVector::RightVector);
			if (dotDirectionForward > cosMaxDirectionAngleError)
			{
				currentDirection = EDirectionName::kDN_Forward;
			}
			else if (dotDirectionForward < -cosMaxDirectionAngleError)
			{
				currentDirection = EDirectionName::kDN_Backward;
			}
			else
			{
				//Stick on horizontal, ie. left or right is ([-1 to 1], 0, 0). So dot with (-1, 0, 0) to get left or right
				float dotDirectionLeft = FVector::DotProduct(direction_targetSpace, FVector::BackwardVector);
				if (dotDirectionLeft > cosMaxDirectionAngleError)
				{
					currentDirection = EDirectionName::kDN_Left;
				}
				else if (dotDirectionLeft < -cosMaxDirectionAngleError)
				{
					currentDirection = EDirectionName::kDN_Right;
				}
				else
				{
					currentDirection = EDirectionName::kDN_None;
				}
			}

			switch (determinedAttackDirection)
			{
			case EDirectionName::kDN_None:
				if (currentDirection == EDirectionName::kDN_None)
				{
					bDirectionFinalized = true;
				}
				else
				{
					determinedAttackDirection = currentDirection;
				}
				break;

			case EDirectionName::kDN_Forward:
				if (currentDirection == EDirectionName::kDN_Backward)
				{
					determinedAttackDirection = EDirectionName::kDN_BackwardToForward;
					bDirectionFinalized = true;
				}
				break;

			case EDirectionName::kDN_Backward:
				if (currentDirection == EDirectionName::kDN_Forward)
				{
					determinedAttackDirection = EDirectionName::kDN_ForwardToBackward;
					bDirectionFinalized = true;
				}
				break;

			case EDirectionName::kDN_Left:
				if (currentDirection == EDirectionName::kDN_Right)
				{
					determinedAttackDirection = EDirectionName::kDN_RightToLeft;
					bDirectionFinalized = true;
				}
				break;

			case EDirectionName::kDN_Right:
				if (currentDirection == EDirectionName::kDN_Left)
				{
					determinedAttackDirection = EDirectionName::kDN_LeftToRight;
					bDirectionFinalized = true;
				}
				break;

			default:
				break;
			}

			break;
		}

		default:
		{
			successiveHeldTime = 0.0f;
			break;
		}
		}
	}

	if (determinedAttackDirection == EDirectionName::kDN_None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("numDirections recorded: %i"), directionStates.Num()));
	}
	return determinedAttackDirection;
}

void AttackInputDirectionEvaluator::EvaluateInputBuffer()
{
	UE_LOG(LogTemp, Warning, TEXT("AttackInputDirectionEvaluator::EvaluateInputBuffer()"));
}
