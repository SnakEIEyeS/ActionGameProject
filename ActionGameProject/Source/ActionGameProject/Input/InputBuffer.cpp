#include "InputBuffer.h"

#include "InputCoreTypes.h"
#include "GameFramework\PlayerController.h"
#include "GameFramework\PlayerInput.h"

static const uint32 bufferCapacity = 256;

AInputBuffer::AInputBuffer() : m_pBuffer(new TCircularBuffer<FInputData>(bufferCapacity, FInputData()))
{
	UE_LOG(LogTemp, Warning, TEXT("AInputBuffer Constructor"))

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AInputBuffer::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("AInputBuffer BeginPlay"))
	Super::BeginPlay();

	PrimaryActorTick.RegisterTickFunction(GetLevel());
	SetActorTickEnabled(true);
}

void AInputBuffer::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("InputBuffer Destroyed()"))

	Super::Destroyed();
}

void AInputBuffer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("InputBuffer Tick()"))

	//Read UPlayerInput's MassageVector functions and check their usage
	//Decide between massaged value (maybe final value from GetProcessedVectorKeyValue) or raw value (GetRawVectorKeyValue) for stick input

	//FKeys for each input key are created in InputCoreTypes .h & .cpp struct EKeys

	//Write an instance of FInputData to m_buffer 
	//query keys from UPlayerInput and store required values

	//SHMANE TODO: find a way to ensure we get player controller so this 'if' is avoided
	if (m_pPlayerController)
	{
		FInputData frameInputData(GFrameCounter);

		frameInputData.m_faceButtonBottom = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Bottom);
		frameInputData.m_faceButtonLeft = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Left);
		frameInputData.m_faceButtonTop = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Top);
		frameInputData.m_faceButtonRight = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_FaceButton_Right);
		frameInputData.m_leftShoulder = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_LeftShoulder);
		frameInputData.m_rightShoulder = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_RightShoulder);
		frameInputData.m_leftTrigger = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_LeftTrigger);
		frameInputData.m_rightTrigger = m_pPlayerController->PlayerInput->IsPressed(EKeys::Gamepad_RightTrigger);

		(*m_pBuffer)[m_nextWriteIndex] = frameInputData;
		UE_LOG(LogTemp, Warning, TEXT("Current frame number = %d | InputBuffer last written frame number = %d"), GFrameCounter, (*m_pBuffer)[m_nextWriteIndex].m_frameNumber);
		m_nextWriteIndex = m_pBuffer->GetNextIndex(m_nextWriteIndex);
	}
}

FInputData::FInputData() : m_frameNumber(-1)
{
}

FInputData::FInputData(uint64 frameNumber) : m_frameNumber(frameNumber)
{
}
