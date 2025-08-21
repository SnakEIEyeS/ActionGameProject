// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputEvaluator.h"

/**
 * 
 */
class ACTIONGAMEPROJECT_API AttackInputDirectionEvaluator : public InputEvaluatorBase
{
public:
	AttackInputDirectionEvaluator();
	~AttackInputDirectionEvaluator();

	EDirectionName GetAttackInputDirection(const FTransform& i_targetSpace);

protected:
	virtual void EvaluateInputBuffer() override;

private:
	const float m_evaluationTime = 1.5f;
	const float m_maxTimeToReachExtremes = 0.5f;
	const float m_longHoldThreshold = 0.75f;
	const uint8 m_maxHoldAngleDifference = 40;
	const uint8 m_maxDirectionAngleError = 20;
};
