// CBGameModeBase.cpp
#include "Game/CBGameModeBase.h"
#include "CBGameStateBase.h"
#include "Player/CBPlayerController.h"
#include "EngineUtils.h"
#include "Player/CBPlayerState.h"

void ACBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("[CBGameModeBase]■■■■■DEBUG■■■■■Answer : %s"), *SecretNumberString);

	// 플레이어가 이미 존재하면 첫 번째 플레이어부터 턴 시작
	if (AllPlayerControllers.Num() > 0)
	{
		CurrentTurnPlayerIndex = 0;
		StartTurn();
	}
}

void ACBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	// 1회
	//ACBGameStateBase* CBGameStateBase = GetGameState<ACBGameStateBase>();
	//if (IsValid(CBGameStateBase) == true)
	//{
	//	CBGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	//}
	
	// 2회
	//ACBPlayerController* CBPlayerController = Cast<ACBPlayerController>(NewPlayer);
	//if (IsValid(CBPlayerController) == true)
	//{
	//	AllPlayerControllers.Add(CBPlayerController);
	//}

	ACBPlayerController* CBPlayerController = Cast<ACBPlayerController>(NewPlayer);
	if (IsValid(CBPlayerController) == true)
	{
		// 접속 성공 메시지
		CBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		// 플레이어 리스트에 추가
		AllPlayerControllers.Add(CBPlayerController);

		ACBPlayerState* CBPS = CBPlayerController->GetPlayerState<ACBPlayerState>();
		if (IsValid(CBPS) == true)
		{
			// 서버에 있는 CBPS의 특정 속성 변경 -> Replication
			// "Player" + 현재 접속자 수(몇 번째 플레이어인지)로 지정
			CBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ACBGameStateBase* CBGameStateBase = GetGameState<ACBGameStateBase>();
		if (IsValid(CBGameStateBase) == true)
		{
			// 모든 클라이언트에게 접속 플레이어 이름 브로드캐스트
			CBGameStateBase->MulticastRPCBroadcastLoginMessage(CBPS->PlayerNameString);
		}
	}
}

FString ACBGameModeBase::GenerateSecretNumber()
{// 1~9 사이의 서로 다른 숫자 3개로 구성된 문자열을 랜덤하게 생성해서 반환

	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	// 랜덤 시드 초기화 -> 매번 실행할 때마다 다른 랜덤 결과
	FMath::RandInit(FDateTime::Now().GetTicks());

	// 0보다 큰 숫자만 필터링 (의미 없는 코드)
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	// 3번 반복
	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		// Numbers 배열에서 랜덤한 인덱스를 선택
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);

		// 해당 숫자를 문자열로 Result에 붙이고
		Result.Append(FString::FromInt(Numbers[Index]));

		// 중복 방지를 위해 배열에서 그 숫자를 제거
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ACBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{// 유효한 숫자인지 확인

	/*
		조건:
		1. 3자리인지
		2. 각 자리가 1~9인지 (0은 제외)
	*/
	// 반환값 초기화. 기본은 false (즉, 게임 불가능)
	bool bCanPlay = false;

	do {
		//1. 문자열 길이가 3자리가 아니면 탈락
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			// 2. 숫자가 아니거나 0이면 탈락
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			// 숫자이고 0이 아니면 TSet에 추가
			// Set이므로 자동으로 중복 제거
			// -> 그러나 중복된 숫자를 검사하는 로직이 실제로 없음 (추가 필요)
			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}
		
		//중복 없는 숫자만 허용하려면 추가 검사 필요
		// 3. 중복 숫자가 있으면 탈락 (Set 크기와 원래 길이 비교)
		if (UniqueDigits.Num() != InNumberString.Len())
		{
			break;
		}

		// 조건을 통과하면 게임 가능
		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ACBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{// 추측 결과 판정 (S/B/OUT)

	/*
		InSecretNumberString : 정답 숫자 문자열
		InGuessNumberString : 플레이어가 추측한 숫자 문자열

		정확히 맞는 자리와 숫자의 개수 (스트라이크)
		숫자만 맞고 자리 다른 개수 (볼)
		둘 다 없을 경우 "OUT", 그렇지 않으면 스트라이크 수와 볼 수를 반환
	*/

	int32 StrikeCount = 0, BallCount = 0;

	// 3자리 숫자 각각을 검사
	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{// 정답의 i번째 숫자와 플레이어의 i번째 숫자가 같으면
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{// 플레이어 숫자가 정답 숫자에 포함되어 있으면
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ACBGameModeBase::ResetGame(const FString& GameEndMessage, int32 StartingPlayerIndex)
{// 게임 리셋 및 새로운 라운드 준비

	// 모든 플레이어 턴 타이머 정지
	for (ACBPlayerController* CBPlayerController : AllPlayerControllers)
	{
		ACBPlayerState* CBPS = CBPlayerController->GetPlayerState<ACBPlayerState>();
		if (IsValid(CBPS))
		{
			CBPS->StopTurnTimer();
		}
	}

	// 승리자 또는 무승부 메시지를 모든 플레이어에게 채팅창으로 출력
	for (ACBPlayerController* CBPlayerController : AllPlayerControllers)
	{
		if (IsValid(CBPlayerController))
		{
			if (!GameEndMessage.IsEmpty())
			{
				CBPlayerController->ClientRPCPrintChatMessageString(GameEndMessage);
			}

			// 새로운 게임 시작 메시지
			CBPlayerController->ClientRPCPrintChatMessageString(TEXT("★★★★★ NEW GAME START! All attempts have been reset! ★★★★★"));
		}
	}

	// 모든 플레이어의 시도 횟수 초기화
	//for (const auto& CBPlayerController : AllPlayerControllers)
	for (ACBPlayerController* CBPlayerController : AllPlayerControllers)
	{
		ACBPlayerState* CBPS = CBPlayerController->GetPlayerState<ACBPlayerState>();
		if (IsValid(CBPS) == true)
		{
			CBPS->CurrentGuessCount = 0;
		}
	}

	// 게임 종료 상태 확인값 초기화
	bGameEnded = false;

	// 새로운 비밀 숫자 생성
	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("[CBGameModeBase]■■■■■DEBUG■■■■■New Answer : %s"), *SecretNumberString);

	// 새 게임 턴 시작
	CurrentTurnPlayerIndex = StartingPlayerIndex;
	UE_LOG(LogTemp, Warning, TEXT("[CBGameModeBase] ResetGame called. StartingPlayerIndex: %d"), StartingPlayerIndex);

	// 리셋 후 첫 턴 시작 여부 활성화
	bIsFirstTurnAfterReset = true;

	// 1초 후 턴 시작
	FTimerHandle StartTurnDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTurnDelayHandle, this, &ACBGameModeBase::StartTurn, 1.0f, false);
}

void ACBGameModeBase::JudgeGame(ACBPlayerController* InChattingPlayerController, int InStrikeCount)
{// 정답 맞췄는지 판단

	if (3 == InStrikeCount)
	{// 플레이어가 3 스트라이크(정답)를 맞춘 경우
		
		// 플레이어 상태 가져오기
		ACBPlayerState* CBPS = InChattingPlayerController->GetPlayerState<ACBPlayerState>();

		// 승자 메시지 작성
		FString WinnerMessage = FString::Printf(TEXT("※ RESULT : %s has won the game!"), *CBPS->PlayerNameString);

		//// 모든 플레이어에게 알림 메시지를 보냄
		//for (const auto& CBPlayerController : AllPlayerControllers)
		//{
		//	if (IsValid(CBPS) == true)
		//	{
		//		FString CombinedMessageString = CBPS->PlayerNameString + TEXT(" has won the game.");
		//		CBPlayerController->NotificationText = FText::FromString(CombinedMessageString);
		//	}
		//}

		// 게임 종료 상태로 설정
		bGameEnded = true;

		// 이긴 사람의 인덱스를 다음 턴 시작 인덱스로 설정
		int32 WinnerIndex = AllPlayerControllers.IndexOfByKey(InChattingPlayerController);
		if (WinnerIndex != INDEX_NONE)
		{
			ResetGame(WinnerMessage, WinnerIndex);
		}
		else
		{// 못 찾을 경우 기본값
			ResetGame(WinnerMessage, 0);
		}
	}
	else
	{
		bool bIsDraw = true;

		// 모든 플레이어의 시도 횟수를 검사
		//for (const auto& CBPlayerController : AllPlayerControllers)
		for (ACBPlayerController* CBPlayerController : AllPlayerControllers)
		{
			ACBPlayerState* CBPS = CBPlayerController->GetPlayerState<ACBPlayerState>();
			if (IsValid(CBPS) == true)
			{
				// 한 명이라도 최대 시도 횟수보다 적게 시도했다면 무승부 아님
				if (CBPS->CurrentGuessCount < CBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{// 무승부인 경우

			FString DrawMessage = TEXT("※ RESULT : Draw!");
			// 게임 초기화
			ResetGame(DrawMessage);
		}
	}
}

bool ACBGameModeBase::IsDrawCondition() const
{// 무승부 조건 검사

	for (ACBPlayerController* CBPlayerController : AllPlayerControllers)
	{
		ACBPlayerState* OtherPS = CBPlayerController->GetPlayerState<ACBPlayerState>();
		if (IsValid(OtherPS) && OtherPS->CurrentGuessCount < OtherPS->MaxGuessCount)
		{
			return false;
		}
	}
	return true;
}

void ACBGameModeBase::IncreaseGuessCount(ACBPlayerController* InChattingPlayerController)
{// 시도 횟수 증가

	ACBPlayerState* CBPS = InChattingPlayerController->GetPlayerState<ACBPlayerState>();
	if (IsValid(CBPS) == true)
	{
		// 최대 시도 횟수에 도달하지 않은 경우에만 시도 횟수 증가
		if (CBPS->CurrentGuessCount < CBPS->MaxGuessCount)
		{
			CBPS->CurrentGuessCount++;
		}

		// 남은 기회가 하나 남았을 때 경고 메시지 출력
		if (CBPS->CurrentGuessCount == CBPS->MaxGuessCount - 1)
		{
			const FString WarningMessage = TEXT("WARNING! Last attempt remaining!");
			InChattingPlayerController->ClientRPCPrintChatMessageString(WarningMessage);
			UE_LOG(LogTemp, Warning, TEXT("[CBGameModeBase]%s has last attempt remaining."), *CBPS->PlayerNameString);
		}
		
		// 최대 시도 횟수에 도달했을 때 알림 메시지 출력
		if (CBPS->CurrentGuessCount == CBPS->MaxGuessCount)
		{
			const FString NoGuessesLeftMessage = TEXT("NO MORE guesses left!");
			InChattingPlayerController->ClientRPCPrintChatMessageString(NoGuessesLeftMessage);
			UE_LOG(LogTemp, Warning, TEXT("[CBGameModeBase]%s reached max guess attempts."), *CBPS->PlayerNameString);
		}
	}
}

void ACBGameModeBase::PrintChatMessageString(ACBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	//====================================================
	// 공통부
	//====================================================
	// 플레이어 상태 가져오기
	ACBPlayerState* CBPS = InChattingPlayerController->GetPlayerState<ACBPlayerState>();
	if (!IsValid(CBPS)) return;

	// 플레이어 정보 문자열
	FString PlayerInfoString = FString::Printf(TEXT("%s (Attempt: %d / %d)"),
		*CBPS->PlayerNameString, CBPS->CurrentGuessCount, CBPS->MaxGuessCount);

	// 최종 출력 문자열
	FString CombinedMessageString;

	/*
		최종 조립된 채팅 메시지를 마지막에 공통부로 출력할 경우,
		게임 판정 도중 승부가 나면,
		게임이 초기화되면서 리셋 메시지가 먼저 출력된 다음
		플레이어의 채팅 메시지가 출력되는 문제가 발생.
		-> 공통부로 출력하지 않고, 개별부에서 출력해야 함.
	*/

	//====================================================
	// 개별부
	//====================================================
	if (IsGuessNumberString(GuessNumberString) == true)
	{// 야구 게임 시도 채팅인 경우

		// 현재 턴 플레이어만 입력 허용
		if (InChattingPlayerController != GetCurrentTurnPlayerController())
		{
			// 아닌 경우 무시 또는 경고 메시지 출력
			InChattingPlayerController->NotificationText = FText::FromString(TEXT("It's not your turn! Please wait."));
			return;
		}

		// 최대 시도 횟수 초과 방지
		if (CBPS->CurrentGuessCount >= CBPS->MaxGuessCount)
		{
			return;
		}

		// 시도 횟수 증가
		IncreaseGuessCount(InChattingPlayerController);

		// 판정
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		int32 StrikeCount = 0;
		if (JudgeResultString.Len() > 0 && FChar::IsDigit(JudgeResultString[0]))
		{// 첫 글자가 숫자인 경우에만 스트라이크 파싱
			StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		}

		// 플레이어 문자열 재조립 : 증가된 시도 횟수 기준으로 다시 생성
		PlayerInfoString = FString::Printf(TEXT("%s (Attempt: %d / %d)"),
			*CBPS->PlayerNameString, CBPS->CurrentGuessCount, CBPS->MaxGuessCount);

		// 최종 출력 문자열 조립
		CombinedMessageString = FString::Printf(TEXT("%s: %s -> %s"),
			*PlayerInfoString, *InChatMessageString, *JudgeResultString);

		// 채팅 출력 (모든 클라이언트에게)
		for (TActorIterator<ACBPlayerController> It(GetWorld()); It; ++It)
		{
			ACBPlayerController* CBPlayerController = *It;
			if (IsValid(CBPlayerController) == true)
			{
				CBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		// 판정 처리
		JudgeGame(InChattingPlayerController, StrikeCount);

		// 턴 종료 (정답 맞추거나 아니면 그냥 턴 종료)
		// 게임이 끝났으면 EndTurn() 호출하지 않음
		if (!bGameEnded)
		{
			EndTurn();
		}

		//FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		//IncreaseGuessCount(InChattingPlayerController);
		//for (TActorIterator<ACBPlayerController> It(GetWorld()); It; ++It)
		//{
		//	ACBPlayerController* CBPlayerController = *It;
		//	if (IsValid(CBPlayerController) == true)
		//	{
		//		FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
		//		CBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

		//		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		//		JudgeGame(InChattingPlayerController, StrikeCount);
		//	}
		//}
	}
	else
	{// 게임이 아닌 일반 채팅인 경우
		// 일반 채팅 메시지 구성
		CombinedMessageString = FString::Printf(TEXT("%s: %s"),
			*PlayerInfoString, *InChatMessageString);

		// 채팅 출력 (모든 클라이언트에게)
		for (TActorIterator<ACBPlayerController> It(GetWorld()); It; ++It)
		{
			ACBPlayerController* CBPlayerController = *It;
			if (IsValid(CBPlayerController) == true)
			{
				CBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
}

ACBPlayerController* ACBGameModeBase::GetCurrentTurnPlayerController()
{
	if (AllPlayerControllers.Num() == 0) return nullptr;

	if (CurrentTurnPlayerIndex >= AllPlayerControllers.Num())
	{
		CurrentTurnPlayerIndex = 0;
	}

	// 현재 턴의 플레이어 컨트롤러 반환
	return AllPlayerControllers[CurrentTurnPlayerIndex];
}

void ACBGameModeBase::StartTurn()
{
	// 현재 턴의 플레이어 컨트롤러 가져오기
	ACBPlayerController* CurrentPlayer = GetCurrentTurnPlayerController();
	if (!IsValid(CurrentPlayer)) return;

	// 플레이어 상태 가져오기
	ACBPlayerState* CBPS = CurrentPlayer->GetPlayerState<ACBPlayerState>();
	if (!IsValid(CBPS)) return;

	// 플레이어 턴 타이머 시작
	CBPS->StartTurnTimer();

	// 모든 플레이어에게 턴 정보 알림
	for (ACBPlayerController* PC : AllPlayerControllers)
	{
		if (IsValid(PC))
		{
			if (PC == CurrentPlayer)
			{// 본인의 턴
				PC->NotificationText = FText::FromString(TEXT("Your turn!"));
			}
			else
			{// 다른 플레이어의 턴
				PC->NotificationText = FText::FromString(FString::Printf(TEXT("Waiting for %s's turn"), *CBPS->PlayerNameString));
			}
		}
	}
}

void ACBGameModeBase::EndTurn(bool bTimeout)
{
	// 이미 게임이 끝났으면 더 이상 처리하지 않음 (정답 맞춘 경우, 무승부 등)
	if (bGameEnded)
	{
		return;
	}

	// 현재 턴의 플레이어 가져오기
	ACBPlayerController* CurrentPlayer = GetCurrentTurnPlayerController();
	if (!IsValid(CurrentPlayer)) return;

	// 플레이어 상태 가져오기
	ACBPlayerState* CBPS = CurrentPlayer->GetPlayerState<ACBPlayerState>();
	if (!IsValid(CBPS)) return;

	// 타이머 중지
	CBPS->StopTurnTimer();

    // 타임아웃 시 처리 (ex: 자동 패스, 시도 횟수 증가 등)
    if (bTimeout)
    {
		// 만료 알림 메시지 출력
        CurrentPlayer->NotificationText = FText::FromString(TEXT("Time EXPIRED! Your turn is skipped."));

        // 시도 횟수 증가
        IncreaseGuessCount(CurrentPlayer);

        // 남은 시도 횟수 계산
        int32 RemainingAttempts = CBPS->MaxGuessCount - CBPS->CurrentGuessCount;

        // 남은 시도 횟수 출력 메시지 추가
        FString RemainingAttemptsMessage = FString::Printf(TEXT("Remaining attempts: %d"), RemainingAttempts);
        CurrentPlayer->ClientRPCPrintChatMessageString(RemainingAttemptsMessage);

        UE_LOG(LogTemp, Warning, TEXT("[CBGameModeBase]%s turn timed out. Remaining attempts: %d"), *CBPS->PlayerNameString, RemainingAttempts);
    }

	// 무승부 판단 추가 (모든 플레이어가 기회를 다 썼는지 확인)
	if (IsDrawCondition())
	{
		bGameEnded = true;
		ResetGame(TEXT("※ RESULT : Draw!"));
		return;
	}

	// 다음 턴으로 진행
	AdvanceTurn();
}

void ACBGameModeBase::AdvanceTurn()
{
	// 플레이어가 아무도 없으면 무시
	if (AllPlayerControllers.Num() == 0) return;

	if (bIsFirstTurnAfterReset)
	{// 게임이 리셋된 직후 첫 턴인 경우

		bIsFirstTurnAfterReset = false;  // 첫 턴이면 인덱스 증가 안 함
		StartTurn();
	}
	else
	{
		// 최대 시도 횟수를 초과하지 않은 플레이어를 찾을 때까지 인덱스 증가
		int32 Attempts = 0;
		do
		{// 인덱스를 순환하면서 다음 플레이어를 탐색

			CurrentTurnPlayerIndex = (CurrentTurnPlayerIndex + 1) % AllPlayerControllers.Num();

			ACBPlayerController* NextPlayer = AllPlayerControllers[CurrentTurnPlayerIndex];
			if (IsValid(NextPlayer))
			{
				ACBPlayerState* CBPS = NextPlayer->GetPlayerState<ACBPlayerState>();
				if (IsValid(CBPS))
				{
					// 최대 시도 횟수에 도달하지 않은 플레이어를 찾으면 턴 시작
					if (CBPS->CurrentGuessCount < CBPS->MaxGuessCount)
					{
						StartTurn();
						return;
					}
				}
			}

			Attempts++;

		} while (Attempts < AllPlayerControllers.Num()); // 모든 플레이어 순회할 때까지 반복
	}
}
