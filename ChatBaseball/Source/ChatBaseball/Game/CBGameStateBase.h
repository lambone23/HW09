// CBGameStateBase.h
/*
	전체 게임의 상태를 관리
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CBGameStateBase.generated.h"

UCLASS()
class CHATBASEBALL_API ACBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	// 모든 클라이언트에게 로그인 메시지를 브로드캐스트하는 멀티캐스트 RPC 함수
	// 이 함수는 서버에서 호출되며, 모든 클라이언트에게 동일한 메시지를 보냄.
	// Reliable로 선언되어 있으므로 반드시 전송됨.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
	
};
