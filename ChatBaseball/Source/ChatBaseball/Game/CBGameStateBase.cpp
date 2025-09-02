// CBGameStateBase.cpp
#include "Game/CBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CBPlayerController.h"

void ACBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	// 클라이언트 측에서만 실행되도록 분기 (서버에서는 실행되지 않음)
	if (HasAuthority() == false)
	{// 서버가 아닌 경우, 클라이언트인 경우
		// 클라이언트에서 내 컴퓨터에는 내 컨트롤러만 있으므로 0번은 로컬 컨트롤러
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ACBPlayerController* CBPC = Cast<ACBPlayerController>(PC);
			if (IsValid(CBPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				CBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
