// CBPlayerController.cpp
#include "Player/CBPlayerController.h"
#include "UI/CBChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ChatBaseball.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/CBGameModeBase.h"
#include "CBPlayerState.h"
#include "Net/UnrealNetwork.h"

ACBPlayerController::ACBPlayerController()
{
	bReplicates = true;
}

void ACBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*
		서버가 클라이언트에 보내는 RPC
		호출 주체 : 서버
		실행 위치 : 클라이언트
		목적 : 서버가 클라이언트에 메시지 보여줌
		동작 : 클라이언트의 PrintChatMessageString()을 실행
	*/
	PrintChatMessageString(InChatMessageString);
}

void ACBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*
		클라이언트가 서버에 보내는 RPC
		호출 주체 : 클라이언트
		실행 위치 : 서버
		목적 : 클라이언트가 서버에 메시지 보냄
		동작 : 클라이언트가 서버에 채팅을 보낼 때, 서버가 GameMode의 처리 함수를 호출
	*/
	// GameModeBase에서 아래와 같은 내용이 추가되었으므로 여기서는 더이상 수행하지 않음
	//for (TActorIterator<ACBPlayerController> It(GetWorld()); It; ++It)
	//{
	//	ACBPlayerController* CBPlayerController = *It;
	//	if (IsValid(CBPlayerController) == true)
	//	{
	//		CBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
	//	}
	//}
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ACBGameModeBase* CBGM = Cast<ACBGameModeBase>(GM);
		if (IsValid(CBGM) == true)
		{
			CBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void ACBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UCBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ACBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ACBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	//PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)
	{
		//ServerRPCPrintChatMessageString(InChatMessageString);

		ACBPlayerState* CBPS = GetPlayerState<ACBPlayerState>();
		if (IsValid(CBPS) == true)
		{
			//FString CombinedMessageString = CBPS->PlayerNameString + TEXT(": ") + InChatMessageString;
			
			// 이슈 : PlayerInfoString()에서 값을 가져올 때, 시도 회차가 바로 업데이트 되지 않는 상태로 출력되는 문제
			//FString CombinedMessageString = CBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			//ServerRPCPrintChatMessageString(CombinedMessageString);

			ServerRPCPrintChatMessageString(ChatMessageString);
		}
	}
}

void ACBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	//UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

	/*
		문제 상황이 생기면,
		아래와 같은 로깅 함수로 다양한 변수의 값들과 함수이름을 확인해서 문제의 원인을 적극적으로 찾기!
	*/
	//FString NetModeString = ChatBaseballFunctionLibrary::GetNetModeString(this);
	//FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	//ChatBaseballFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);

	ChatBaseballFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}
