// CBPlayerState.cpp
#include "Player/CBPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/CBPlayerController.h"
#include "Game/CBGameModeBase.h"

ACBPlayerState::ACBPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
	, TurnTimeLimit(10.0f)
{
	// 네트워크를 통해 동기화될 객체임을 명시
	bReplicates = true;
}

void ACBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
	/*
		Q. MaxGuessCount가 프로퍼티 레플리케이션될 필요가 있을까?
		A:
		클라이언트에서 UI 판단 또는 제한 조건 등에 쓰인다면 필요함.
		예를 들어 입력 제한 값으로 클라이언트에서 이 값을 기반으로 로직을 실행한다면 필요할 수도 있음.
		단순히 서버만 사용하는 값이라면 굳이 동기화할 필요는 없음.
	*/
}

void ACBPlayerState::Client_UpdateTurnTimeRemaining_Implementation(float TimeRemaining)
{
	// 클라이언트에서 남은 시간을 NotificationText에 표시
	FString TimeMessage = FString::Printf(TEXT("Time remaining: %.1f seconds"), TimeRemaining);

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (IsValid(PC))
	{
		ACBPlayerController* CBPC = Cast<ACBPlayerController>(PC);
		if (IsValid(CBPC))
		{
			CBPC->NotificationText = FText::FromString(TimeMessage);
		}
	}
}

FString ACBPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT(" (시도 회차: ") + FString::FromInt(CurrentGuessCount)
		+ TEXT(" / ") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

void ACBPlayerState::StartTurnTimer()
{// 턴 타이머 시작

	// 이미 타이머가 작동 중이거나 서버가 아니면 무시
	if (bIsTurnTimerActive || !HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 제한 시간 뒤에 OnTurnTimerExpired 호출
	World->GetTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ACBPlayerState::OnTurnTimerExpired,
		TurnTimeLimit,
		false // 반복 없음
	);

	// 1초마다 남은 시간 갱신 (클라이언트에게 전달)
	World->GetTimerManager().SetTimer(
		TurnTimerTickHandle,
		this,
		&ACBPlayerState::BroadcastTurnTimeRemaining,
		1.0f, // 1초마다
		true
	);

	// 타이머 활성화 상태 설정
	bIsTurnTimerActive = true;
	UE_LOG(LogTemp, Log, TEXT("[CBPlayerState][Timer] Turn timer started for %s (%f sec)"), *PlayerNameString, TurnTimeLimit);
}

void ACBPlayerState::StopTurnTimer()
{// 턴 타이머 정지

	if (!HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 두 개의 타이머 모두 정지
	World->GetTimerManager().ClearTimer(TurnTimerHandle);
	World->GetTimerManager().ClearTimer(TurnTimerTickHandle);

	// 타이머 비활성화 상태 설정
	bIsTurnTimerActive = false;

	UE_LOG(LogTemp, Log, TEXT("[CBPlayerState][Timer] Turn timer stopped for %s"), *PlayerNameString);
}

void ACBPlayerState::OnTurnTimerExpired()
{// 턴 제한 시간이 초과되었을 때 호출

	StopTurnTimer(); // 모든 타이머 정지

	UE_LOG(LogTemp, Warning, TEXT("[CBPlayerState][Timer] Turn expired for %s"), *PlayerNameString);

	Client_UpdateTurnTimeRemaining(0.f); // "0초"로 보여주기

	// GameMode에 타임아웃 사실을 알리고 턴 종료 처리
	ACBGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ACBGameModeBase>() : nullptr;
	if (IsValid(GM))
	{
		GM->EndTurn(true); // true: 시간 초과로 인한 종료
	}
}

void ACBPlayerState::BroadcastTurnTimeRemaining()
{// 남은 시간 클라이언트에게 주기적으로 전송 (매 1초마다 호출됨)

	if (!HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 현재 TurnTimer의 남은 시간 가져오기
	float TimeRemaining = World->GetTimerManager().GetTimerRemaining(TurnTimerHandle);

	// 클라이언트에게 시간 전송
	Client_UpdateTurnTimeRemaining(TimeRemaining);
}
