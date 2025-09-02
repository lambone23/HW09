// CBPlayerState.h
/*
	플레이어 개별 상태를 관리하는 클래스
	플레이어 이름, 남은 시도 횟수, 턴 타이머 관리 기능 등
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CBPlayerState.generated.h"

UCLASS()
class CHATBASEBALL_API ACBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACBPlayerState();

	// 네트워크 동기화를 위한 변수 등록
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 클라이언트에게 남은 턴 시간을 전달
	UFUNCTION(Client, Reliable)
	void Client_UpdateTurnTimeRemaining(float TimeRemaining);

	// 플레이어 정보 문자열 반환
	FString GetPlayerInfoString();

	// 턴 타이머 제어 함수들
	void StartTurnTimer();				// 턴 시작 시 타이머 시작
	void OnTurnTimerExpired();			// 턴 제한 시간 초과 시 호출
	void StopTurnTimer();				// 턴 중단 또는 종료 시 타이머 정지
	void BroadcastTurnTimeRemaining();	// 남은 시간 지속 브로드캐스트 (틱 단위)

protected:
	// 턴 타이머를 위한 핸들 (제한 시간 도달 시 호출용)
	FTimerHandle TurnTimerHandle;

	// 남은 시간 표시를 위한 틱 타이머 핸들 (주기적으로 남은 시간 갱신)
	FTimerHandle TurnTimerTickHandle;

	// 현재 턴 타이머가 활성화 상태인지 여부
	bool bIsTurnTimerActive;

public:
	// 플레이어 이름
	UPROPERTY(Replicated)
	FString PlayerNameString;

	// 현재까지의 시도 횟수
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	// 최대 허용 시도 횟수
	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	// 한 턴에 허용된 제한 시간 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn")
	float TurnTimeLimit;
	
};
