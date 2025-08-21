#include "InputBuffer.h"

#include "InputCoreTypes.h"
#include "GameFramework\PlayerController.h"
#include "GameFramework\PlayerInput.h"

static const uint32 bufferCapacity = 256;

AInputBuffer::AInputBuffer() : m_pBuffer(new TCircularBuffer<FInputData>(bufferCapacity, FInputData()))
{
	//UE_LOG(LogTemp, Warning, TEXT("AInputBuffer Constructor"))

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.SetTickFunctionEnable(false);
}

void AInputBuffer::BeginPlay()
{
	//UE_LOG(LogTemp, Warning, TEXT("AInputBuffer BeginPlay"))
	Super::BeginPlay();

	//PrimaryActorTick.RegisterTickFunction(GetLevel());
	SetActorTickEnabled(false);
}

void AInputBuffer::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("InputBuffer Destroyed()"))

	Super::Destroyed();
}

void AInputBuffer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UE_LOG(LogTemp, Warning, TEXT("InputBuffer Tick()"))

	//FKeys for each input key are created in InputCoreTypes .h & .cpp struct EKeys

	//SHMANE TODO: find a way to ensure we get player controller so this 'if' is avoided
	if (m_pPlayerController)
	{
		FInputData frameInputData(GFrameCounter, GetWorld()->RealTimeSeconds);

		frameInputData.m_faceButtonBottom = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Bottom);
		frameInputData.m_faceButtonLeft = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Left);
		frameInputData.m_faceButtonTop = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Top);
		frameInputData.m_faceButtonRight = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Right);
		frameInputData.m_leftShoulder = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_LeftShoulder);
		frameInputData.m_rightShoulder = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_RightShoulder);
		frameInputData.m_leftTrigger = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_LeftTrigger);
		frameInputData.m_rightTrigger = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_RightTrigger);

		frameInputData.m_leftThumbstick_X = m_pPlayerController->PlayerInput->GetRawVectorKeyValue(EKeys::Gamepad_LeftX);
		frameInputData.m_leftThumbstick_Y = m_pPlayerController->PlayerInput->GetRawVectorKeyValue(EKeys::Gamepad_LeftY);
		frameInputData.m_rightThumbstick_X = m_pPlayerController->PlayerInput->GetRawVectorKeyValue(EKeys::Gamepad_RightX);
		frameInputData.m_rightThumbstick_Y = m_pPlayerController->PlayerInput->GetRawVectorKeyValue(EKeys::Gamepad_RightY);

		frameInputData.m_leftThumbstick_DistanceSquared = FMath::Square(frameInputData.m_leftThumbstick_X.X) + FMath::Square(frameInputData.m_leftThumbstick_Y.X);
		frameInputData.m_leftThumbstick_Distance = FMath::Sqrt(frameInputData.m_leftThumbstick_DistanceSquared);

		(*m_pBuffer)[m_nextWriteIndex] = frameInputData;
		//UE_LOG(LogTemp, Warning, TEXT("Current frame number = %d | InputBuffer last written frame number = %d"), GFrameCounter, (*m_pBuffer)[m_nextWriteIndex].m_frameNumber);
		m_nextWriteIndex = m_pBuffer->GetNextIndex(m_nextWriteIndex);

		/*UE_LOG(LogTemp, Error, TEXT("[InputBuffer] LeftStick values - X=%s , Y=%s"), 
			*frameInputData.m_leftThumbstick_X.ToString(), *frameInputData.m_leftThumbstick_Y.ToString());*/

		float Distance = FMath::Sqrt(frameInputData.m_leftThumbstick_DistanceSquared);
		/*UE_LOG(LogTemp, Error, TEXT("[InputBuffer] LeftStick distance travelled - DistanceSquared=%f , Distance=%f"),
			frameInputData.m_leftThumbstick_DistanceSquared, Distance);*/
		
		//UE_LOG(LogTemp, Error, TEXT("Gamepad_LeftX vector value = %s"), *frameInputData.m_leftThumbstick_X.ToString());
		//UE_LOG(LogTemp, Error, TEXT("Gamepad_LeftY vector value = %s"), *frameInputData.m_leftThumbstick_Y.ToString());
		//UE_LOG(LogTemp, Error, TEXT("Gamepad_RightX vector value = %s"), *frameInputData.m_rightThumbstick_X.ToString());
		//UE_LOG(LogTemp, Error, TEXT("Gamepad_RightY vector value = %s"), *frameInputData.m_rightThumbstick_Y.ToString());
	}

}

FInputData::FInputData() : m_frameNumber(-1), m_timeStamp(0.f)
{
}

FInputData::FInputData(uint64 frameNumber, float timeStamp) : m_frameNumber(frameNumber), m_timeStamp(timeStamp)
{
}
