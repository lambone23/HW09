// ChatBaseball.h

#pragma once

#include "CoreMinimal.h"

class ChatBaseballFunctionLibrary
{
public:
	static void MyPrintString(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		if (IsValid(GEngine) == true && IsValid(InWorldContextActor) == true)
		{
			if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)
			{// Clinent 또는 ListenServer 인 경우에만 화면에 출력
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
			}
			else
			{// Dedicated Server, 화면에 출력 안되므로 로그로 출력
				UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
			}
		}
	}

	static FString GetNetModeString(const AActor* InWorldContextActor)
	{
		FString NetModeString = TEXT("None");

		if (IsValid(InWorldContextActor) == true)
		{
			ENetMode NetMode = InWorldContextActor->GetNetMode();
			if (NetMode == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else
			{
				if (NetMode == NM_Standalone)
				{
					NetModeString = TEXT("StandAlone");
				}
				else
				{// ListenServer 또는 DedicatedServer인 경우
					NetModeString = TEXT("Server");
				}
			}
		}

		return NetModeString;
	}

};
