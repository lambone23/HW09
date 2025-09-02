# HW09
# ⚾ Chat Baseball Game (Unreal Engine 5 C++)

> **Chat Baseball Game**은 **언리얼 엔진 5 C++** 기반의 멀티플레이어 숫자 야구 게임입니다.  
> 플레이어는 채팅창을 통해 **3자리 숫자**를 추측하며, 먼저 정답을 맞힌 플레이어가 승리합니다.  
> 게임은 **턴 기반**, **제한 시간**, **자동 리셋** 기능을 포함하며, **승리한 플레이어가 다음 게임의 선턴**이 됩니다.  
> _Unreal Engine의 Dedicated Server 및 클라이언트-서버 통신 구조를 이해하고 실습하기 위한 목적으로 제작한 프로젝트입니다._
---

## 📌 게임 개요
- **장르** : 턴제 숫자 추측 게임 (숫자 야구) / Turn-based number guessing game (similar to "Bulls and Cows")
- **플랫폼** : Unreal Engine 5.5
- **언어** : C++
- **구조** : 데디케이티드 서버 기반의 클라이언트-서버 멀티플레이어 구조
  - Unreal Engine의 `Dedicated Server` 모드 사용
  - 서버에서 게임 로직, 상태, 턴 처리 전담
  - 클라이언트는 입력 및 UI 처리
  - 네트워크 실시간 판정 및 동기화 실습 목적
- **제작 기간** : 2일 [2025/09/01 - 2025/09/02]
---

## 🎮 주요 기능
- 멀티플레이어 지원 (서버-클라이언트 구조)
- 턴 기반 게임 진행 (순차적으로 추측)
- 정답 판정 알고리즘 (스트라이크, 볼, 아웃)
- 자동 리셋 (승리 / 무승부 후 재시작)
- **턴 타이머** (시간 초과 시 자동 패스)
- **승자 우선 시작** (이긴 플레이어가 다음 게임 선턴)

---

## 🚀 도전 기능 (게임 고도화)
- 턴 제어 기능
- 타임아웃 처리
- 승리자 우선 시작 로직
- 최대 시도 횟수 직전/도달 시 경고 메시지 출력

---

## 📝 게임 규칙
- 정답: **1~9 사이 중복 없는 3자리 숫자**
- 추측 입력: 채팅창에 3자리 숫자 입력
- 판정 결과 예시:
  - `3S0B` → 숫자와 위치 모두 맞음 (**정답**)
  - `1S2B` → 1개는 위치까지 맞고, 2개는 숫자만 맞음
  - `OUT` → 일치하는 숫자 없음
- 게임 종료 조건:
  - 정답자 발생 → 승리
  - 모든 시도 소진 → 무승부
- 다음 게임은 **승리자가 선턴**

---

## ⚙️ 테스트 전 필수 에디터 설정
테스트를 시작하기 전에 아래와 같은 개인 설정이 필요합니다:

- **에디터 개인 설정 열기** : 상단 메뉴에서 '편집' → '에디터 개인설정' 열고 아래 내용 적용  
  <img width="625" height="411" alt="image" src="https://github.com/user-attachments/assets/0cf7f9f7-2b39-4b59-8524-2db30545264a" />

- **PIE | 늦은 조인 허용** : 값 체크 활성화 
  <img width="991" height="211" alt="image" src="https://github.com/user-attachments/assets/424cd18d-3df2-4b67-b893-fe16f649dc5f" />

- **플레이** : '레벨 에디터' → '플레이' 에서 아래 내용 적용
  - **Play in New Window | 항상 맨 위에** : 값 체크 활성화
  - **멀티 플레이어 옵션 | 개별 서버 실행** : 값 체크 활성화
  - **멀티 플레이어 옵션 | Net Mode 플레이 설정** : `Play As Client`로 설정  
  - **멀티 플레이어 옵션 | 단일 프로세스 하 실행** : 값 체크 비활성화    
  <img width="1152" height="430" alt="image" src="https://github.com/user-attachments/assets/9bc4a3d6-ccbf-4945-8870-b31ba1baad39" />

- **2인 이상 클라이언트 접속 방법** : 아래의 버튼을 누른 만큼 클라이언트가 추가되며 게임 중 추가도 가능
  <img width="312" height="115" alt="image" src="https://github.com/user-attachments/assets/8daaad43-703c-4716-93a2-657819475a35" />

> 설정을 마친 후 `Play` 버튼을 누르면 테스트가 가능합니다.

---

## 🧪 테스트 방법
1. 언리얼 에디터에서 프로젝트 실행
2. 두 명 이상의 클라이언트 접속
3. 플레이어가 차례대로 3자리 숫자 입력
4. 판정 메시지 확인 (`1S2B`, `OUT`, `3S0B`)
5. 승리 및 무승부 후 자동 재시작 확인

---

## 🔄 게임 로직 요약 (5단계)
1. **게임 시작**  
   서버(GameMode)가 비밀 숫자 생성 → 첫 플레이어 턴 시작  

2. **채팅 입력**  
   숫자 입력 → 서버 유효성 검사 및 판정  

3. **판정**  
   스트라이크/볼/아웃 계산 → 전체 브로드캐스트  

4. **턴 관리**  
   - 정답 → 게임 종료 및 리셋  
   - 실패 → 다음 플레이어 턴  
   - 타임아웃 → 자동 턴 종료  

5. **게임 종료 및 재시작**  
   - 승자 발생 → 축하 메시지, 승자부터 새 게임 시작  
   - 무승부 → 새 비밀 숫자 생성 후 리셋  

---

## 🏗️ 주요 클래스
- **ACBGameModeBase** : 게임 전체 흐름 제어 (정답 생성, 턴 관리, 판정 등)  
- **ACBGameStateBase** : 전역 상태 공유 및 메시지 멀티캐스트  
- **ACBPlayerController** : 클라이언트 입력 처리 및 서버 통신  
- **ACBPlayerState** : 플레이어 정보 관리 (이름, 시도 횟수, 타이머 등)  
- **UCBChatInput** : 채팅 입력 UI 위젯  
- **UUserWidget (NotificationText)** : 메시지 출력 (턴 알림, 남은 시간 등)  

---

## 🛠️ 사용 언리얼 구조 및 자료형
- `TArray<ACBPlayerController*>` : 플레이어 리스트 관리  
- `TSet<TCHAR>` : 숫자 중복 검사  
- `FString`, `FText` : 문자열 및 메시지 처리  
- `TActorIterator` : 액터 순회 및 브로드캐스트  
- `FTimerHandle`, `GetWorld()->GetTimerManager()` : 턴 타이머 관리  

---

## 🔎 게임 세부 흐름
1. **게임 시작** → 비밀 숫자 생성, 첫 턴 시작  
2. **플레이어 접속** → 리스트 추가, 접속 메시지 전송  
3. **턴 시작** → 타이머 시작, 클라이언트 알림  
4. **채팅 입력** → 유효성 검사, 판정, 결과 브로드캐스트  
5. **타임아웃** → 자동 턴 종료 처리  
6. **판정/종료** → 승리/무승부 판정 후 게임 리셋  
7. **게임 리셋** → 시도 횟수 초기화, 새 숫자 생성, 턴 재설정  

---

## 🖼️ 실행 스크린샷
1. **에디터에서 Play 시작**
    <img width="968" height="207" alt="image" src="https://github.com/user-attachments/assets/f76add82-4f74-4b22-9a2c-20747a52b601" />

2. **클라이언트 추가** : 클라이언트는 2인 이상으로 추가 
   
    <img width="389" height="116" alt="image" src="https://github.com/user-attachments/assets/2f6744c5-4ed1-44b6-b571-7ad5385b29ce" />
  - 2인 (먼저 접속한 플레이어 화면에 새로 접속한 플레이어 알림 출력)
    <img width="1552" height="785" alt="image" src="https://github.com/user-attachments/assets/98cbc3f9-fd50-4926-8f9c-1f40cf9a1e81" />
  - 3인 (먼저 접속한 플레이어 화면에 새로 접속한 플레이어 알림 출력)
    <img width="1553" height="771" alt="image" src="https://github.com/user-attachments/assets/ae7f2ea6-60f8-4c07-bcdb-5a353c82466a" />

3. **게임 시도와 무관한 채팅 입력시** : 시도 횟수 증가하지 않음 
    <img width="1555" height="773" alt="image" src="https://github.com/user-attachments/assets/ec90bd68-47f5-46b6-a334-876d01b34532" />

4. 3자리 숫자를 입력하면 게임이 시작되며, 턴 기반 방식으로 진행
  - 본인 턴의 경우 '턴 타이머' 활성화, 나머지 플레이어는 'Waiting' 문구 출력
    <img width="1558" height="785" alt="image" src="https://github.com/user-attachments/assets/8aae5c94-73c4-436f-b156-c469726a1ccf" />

5. **최대 시도 횟수 직전 또는 도달 시 경고 메시지 출력**
  - 최대 시도 횟수에 도달하면, 게임 추측 입력은 막히고 일반 채팅만 가능
    <img width="784" height="327" alt="image" src="https://github.com/user-attachments/assets/0d2b056c-a394-46c6-860b-8d36ca36a742" />

6. **무승부** : 무승부 결과를 채팅 메시지로 출력 후 게임 리셋 및 재시작
    <img width="1886" height="977" alt="image" src="https://github.com/user-attachments/assets/4072d29f-3d82-4998-8a7e-929887f23176" />

7. **승부 판정** : 승자가 발생한 경우
    <img width="1269" height="784" alt="image" src="https://github.com/user-attachments/assets/68af35bf-edde-4515-accb-f5a4d66b9938" />

