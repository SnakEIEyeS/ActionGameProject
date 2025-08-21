#pragma once

#include "CoreMinimal.h"
#include "Containers/CircularBuffer.h"
#include "InputBuffer.generated.h"

USTRUCT()
struct FInputData
{
	GENERATED_BODY()

	FInputData();
	FInputData(uint64 frameNumber, float timeStamp);

	int64 m_frameNumber;
	float m_timeStamp;

	uint8 m_faceButtonBottom : 1;	//Gamepad_FaceButton_Bottom
	uint8 m_faceButtonLeft : 1;		//Gamepad_FaceButton_Left
	uint8 m_faceButtonTop : 1;		//Gamepad_FaceButton_Top
	uint8 m_faceButtonRight : 1;	//Gamepad_FaceButton_Right

	uint8 m_leftShoulder : 1;		//Gamepad_LeftShoulder
	uint8 m_rightShoulder : 1;		//Gamepad_RightShoulder
	uint8 m_leftTrigger : 1;		//Gamepad_LeftTrigger
	uint8 m_rightTrigger : 1;		//Gamepad_RightTrigger

	//Gamepad_DPad_Up
	//Gamepad_DPad_Left
	//Gamepad_DPad_Right
	//Gamepad_DPad_Down

	//Gamepad_LeftThumbstick
	//Gamepad_RightThumbstick

	//SHMANE TODO these seem to have a value in only 1 component of the vector
	FVector m_leftThumbstick_X;		//Gamepad_LeftX
	FVector m_leftThumbstick_Y;		//Gamepad_LeftY
	FVector m_rightThumbstick_X;	//Gamepad_RightX
	FVector m_rightThumbstick_Y;	//Gamepad_RightY

	float m_leftThumbstick_DistanceSquared;		//Square of Distance on the unit circle that Left Thumbstick can move along
	float m_leftThumbstick_Distance;			//Distance on the unit circle that Left Thumbstick can move along

};

class APlayerController;

//Put this on player controller. This needs to be on player controller so it isn't tied to a character but is tied to a player and their input.
UCLASS()
class ACTIONGAMEPROJECT_API AInputBuffer : public AActor
{
	GENERATED_BODY()

public:
	

	AInputBuffer();

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	FORCEINLINE void SetPlayerController(APlayerController* playerController) {	m_pPlayerController = playerController;	}

	FORCEINLINE TSharedPtr<TCircularBuffer<FInputData>> GetBuffer() const { return m_pBuffer; }
	//TODO Add Get last wite idx
	FORCEINLINE const uint32 GetLastWriteIndex() const { return m_pBuffer->GetPreviousIndex(m_nextWriteIndex); }

	//SHMANE TODO add these to public interface - GetInputDataAtTime(), GetNextInputData(), GetIndexAtTime()
protected:

private:
	TSharedPtr<TCircularBuffer<FInputData>> m_pBuffer;
	APlayerController* m_pPlayerController;	//SHMANE TODO is this garbage collected?
	uint32 m_nextWriteIndex = 0;
};
