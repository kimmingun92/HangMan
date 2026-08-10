# 행맨 게임 (Hangman)

C 언어와 [raylib](https://www.raylib.com/) (+ [raygui](https://github.com/raysan5/raygui))를 활용한 GUI 기반 행맨 게임입니다.

![status](https://img.shields.io/badge/language-C11-blue) ![raylib](https://img.shields.io/badge/graphics-raylib-orange)

## 소개

- 사용자가 직접 플레이하는 **수동 모드**와 프로그램이 자동으로 단어를 추측하는 **자동 모드**를 지원합니다.
- **Easy / Medium / Hard** 3단계 난이도를 선택할 수 있습니다.
- 상태 기반(State Machine) 구조로 화면 흐름과 게임 진행을 관리합니다.

### 핵심 목표

- **상태 기반 프로그램 구조 설계**: 화면 전환과 게임 흐름을 `ScreenState`로 관리
- **통합 상태 관리**: `AppState` 하나로 `screen`, `mode`, `difficulty`, `game`, `auto_elapsed`를 관리
- **수동 / 자동 모드 구현**: 사용자 입력 기반 플레이와 규칙 기반 자동 추측 모두 지원
- **난이도 및 카테고리 시스템**: Easy / Medium / Hard에 따라 단어 풀을 분리
- **UI와 게임 로직 분리**: `main` (UI/흐름) / `game` (로직) / `words` (데이터)로 모듈 분리

## 프로젝트 구조

`src/`(구현부)와 `include/`(헤더)를 분리한 구조입니다. 각 폴더에는 해당 폴더 파일들을 설명하는 `README.md`가 별도로 있습니다.

```
.
├── CMakeLists.txt        # 빌드 설정
├── include/
│   ├── README.md
│   ├── game.h             # GameState 및 game_* 함수 선언
│   ├── types.h             # ScreenState / GameMode / Difficulty 공통 타입
│   └── words/
│       ├── README.md
│       ├── words.h         # WordList / WordCatalog 타입, words_get_catalog 선언
│       ├── animal.h
│       ├── color.h
│       ├── food.h
│       ├── fruit.h
│       └── tech.h          # 카테고리별 단어 데이터
└── src/
    ├── README.md
    ├── main.c              # 화면 흐름, 렌더링, 게임 루프
    ├── game.c              # 게임 상태 관리, 정답 판정, 자동 모드 로직
    └── words/
        ├── README.md
        └── words.c         # 난이도·카테고리별 단어 카탈로그 조립
```

### 모듈 역할

| 파일 | 역할 |
| --- | --- |
| `src/main.c` | 화면 흐름 관리, 상태 전환 처리, 게임 루프 실행, 렌더링 함수 호출 |
| `src/game.c` + `include/game.h` | 게임 상태 관리, 정답 판정 및 오답 처리, 게임 종료 판단, 자동 모드 추측 로직 |
| `include/types.h` | 화면 상태, 게임 모드, 난이도 등 공통 자료형 정의 |
| `src/words/words.c` + `include/words/words.h` | 난이도 및 카테고리별 단어 데이터 관리, 단어 목록 제공 |
| `include/words/animal.h` ~ `tech.h` | 카테고리별 단어 정의 |
| `CMakeLists.txt` | 프로젝트 빌드 설정 (`src/` 소스 컴파일, `include/`를 헤더 경로로 지정) |

### 화면 흐름

```
SCREEN_HOME ──MANUAL/AUTO──▶ SCREEN_MODE ──난이도 선택──▶ SCREEN_PLAY ──승리/실패──▶ SCREEN_RESULT
     ▲              ◀──BACK──────┘                              │                        │
     └────────────────────────MENU───────────────────────────────┘                        │
     ◀─────────────────────────────────HOME─────────────────────────────────────────────┘
                                    RESTART → SCREEN_PLAY(재시작)
```

## 주요 기능

- 🎮 **게임 모드 선택**: Manual Start(수동) / Auto Start(자동)
- 🎯 **난이도 선택**: Easy / Medium / Hard, 난이도별 단어 풀 및 카테고리 우선순위 적용
- 🎲 **카테고리 기반 단어 랜덤 선택**: Fruit / Animal / Color / Food / Tech
- 🔤 단어를 `_ _ _` 형태로 표시, 알파벳 입력(A-Z) 또는 클릭으로 추측
- 🚫 중복 입력 방지
- ❌ 오답 횟수에 따라 행맨 일러스트와 배경이 단계적으로 변화 (표정 변화 포함)
- 🤖 자동 모드: 카테고리별 글자 등장 빈도 + 전역 알파벳 빈도 + 모음/자음 균형 + 남은 목숨을 종합해 점수화하여 최적의 글자를 추측
- 🏆 승리 / 💀 패배 판정 후 RESTART(재시작) / HOME(메인 메뉴) 선택 가능

## 빌드 방법 (CMake)

### 사전 준비

- CMake 3.10 이상
- C11을 지원하는 컴파일러 (gcc/clang 등)
- [raylib](https://github.com/raysan5/raylib) (필수)
- [raygui](https://github.com/raysan5/raygui) (선택 — 없어도 자체 구현 UI 버튼으로 동작)

### 1) Configure

```bash
cmake -S . -B build
```

### 2) Build

```bash
cmake --build build
```

빌드 결과 실행 파일: `build/hangman`

## 실행 방법

```bash
./build/hangman
```

## 게임 조작

| 키 | 기능 |
| --- | --- |
| `A`-`Z` | 문자 입력 (수동 모드) |
| 마우스 클릭 | 화면의 버튼 / 알파벳 클릭 |
| `ESC` | 메인 메뉴로 이동 (플레이 화면 제외) |

## 게임 규칙

1. 카테고리 + 단어를 랜덤으로 선택합니다.
2. 단어는 `_`로 표시됩니다.
3. 맞춘 문자는 해당 위치에 공개됩니다.
4. 틀리면 오답 횟수가 증가하고 행맨 그림이 한 단계 진행됩니다.
5. 오답이 6회에 도달하면 패배합니다.
6. 모든 글자를 맞히면 승리합니다.

## 자동 모드 추측 로직 (요약)

`game_choose_auto_letter()`는 아직 시도하지 않은 알파벳 26개를 모두 평가하여 가장 점수가 높은 글자를 선택합니다. 점수(`score_letter()`)는 다음을 종합합니다.

- 현재 카테고리 + 단어 길이에 맞는 글자 우선순위 (가장 큰 가중치)
- 영어 전체 알파벳 등장 빈도 (보조 기준)
- 모음/자음 등장 비율에 따른 가감점 (초반엔 모음 우선, 이후 균형 조정)
- 남은 목숨이 적을수록 희귀 문자(j, q, x, z, v, k)에 감점을 주어 위험 회피

