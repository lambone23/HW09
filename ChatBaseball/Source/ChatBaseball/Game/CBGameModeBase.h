// CBGameModeBase.h
/*
	게임의 전반적인 흐름(판정, 턴 관리, 리셋 등)을 관리하는 핵심 클래스
	서버 전용 로직만 포함
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CBGameModeBase.generated.h"

class ACBPlayerController;

UCLASS()
class CHATBASEBALL_API ACBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// 로그인 후에 호출되는 함수
	virtual void OnPostLogin(AController* NewPlayer) override;

	//===========================================================
	// 게임 로직 관련 함수
	//===========================================================
	// 정답 숫자 3자리를 무작위로 생성 (중복 없이 1~9)
	FString GenerateSecretNumber();

	// 입력값이 유효한 숫자인지 검사
	bool IsGuessNumberString(const FString& InNumberString);

	// 추측 결과를 판정 (스트라이크 / 볼 / 아웃)
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	
	// 게임 종료 후 초기화 및 새 게임 시작
	void ResetGame(const FString& GameEndMessage, int32 StartingPlayerIndex = 0);

	// 정답인지 확인하고 승리 여부 판단
	void JudgeGame(ACBPlayerController* InChattingPlayerController, int InStrikeCount);

	// 무승부 조건 확인 (모든 플레이어가 기회를 소진한 경우)
	bool IsDrawCondition() const;

	// 시도 횟수 증가 함수
	void IncreaseGuessCount(ACBPlayerController* InChattingPlayerController);

	// 채팅 메시지를 전체 플레이어에게 출력
	void PrintChatMessageString(ACBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	//===========================================================
	// 턴제 관련 함수
	//===========================================================
	// 플레이어 리스트에서 현재 턴 플레이어를 반환
	ACBPlayerController* GetCurrentTurnPlayerController();

	// 턴 시작 함수
	void StartTurn();

	// 턴 종료 함수 (타이머 만료 포함)
	void EndTurn(bool bTimeout = false);

	// 턴 전환 함수
	void AdvanceTurn();

protected:
	// 정답 숫자
	FString SecretNumberString;

	// 접속 중인 모든 플레이어 컨트롤러
	TArray<TObjectPtr<ACBPlayerController>> AllPlayerControllers;

	// 현재 턴 플레이어 인덱스
	int32 CurrentTurnPlayerIndex = 0;

	// 게임 종료 상태 확인값
	UPROPERTY()
	bool bGameEnded = false;

	// 리셋 후 첫 턴인지 여부
	UPROPERTY()
	bool bIsFirstTurnAfterReset = false;
	
};
