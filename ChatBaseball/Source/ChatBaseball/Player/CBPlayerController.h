// CBPlayerController.h
/*
	PlayerController 클래스
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CBPlayerController.generated.h"

class UCBChatInput;
class UUserWidget;

UCLASS()
class CHATBASEBALL_API ACBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACBPlayerController();

	// 서버에서 클라이언트에게 채팅 메시지를 출력하라고 요청하는 RPC
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	// 클라이언트에서 입력한 채팅 메시지를 서버로 전송하는 RPC 
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void BeginPlay() override;

	// 리플리케이션 대상 속성 등록 함수
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 내부적으로 사용하는 채팅 메시지 설정 함수
	void SetChatMessageString(const FString& InChatMessageString);

	// 채팅 메시지를 클라이언트 화면에 출력하는 함수 (로컬)
	void PrintChatMessageString(const FString& InChatMessageString);

public:
	// 서버에서 설정 후 클라이언트에 리플리케이션되는 알림 텍스트
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

protected:
	// 채팅 입력 위젯의 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCBChatInput> ChatInputWidgetClass;

	// 채팅 입력 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UCBChatInput> ChatInputWidgetInstance;

	// 알림 텍스트를 표시할 위젯의 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	// 알림 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	// 채팅 메시지 문자열
	FString ChatMessageString;
	
};
