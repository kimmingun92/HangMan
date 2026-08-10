# include/

공개 헤더 파일들이 위치합니다. 대응되는 구현부(.c)는 `../src/`에 있습니다.

| 파일 | 설명 |
| --- | --- |
| `types.h` | 프로젝트 전체에서 쓰이는 공통 열거형(`ScreenState`, `GameMode`, `Difficulty`)을 정의합니다. |
| `game.h` | 게임 진행 상태를 담는 `GameState` 구조체와 `game_init`, `game_guess`, `game_choose_auto_letter`, `game_letter_used` 함수 원형을 선언합니다. |
| `words/` | 단어 카탈로그 관련 헤더 (`words.h`)와 카테고리별 단어 데이터 헤더들 (`animal.h`, `color.h`, `food.h`, `fruit.h`, `tech.h`) |

## 포함 순서 예시

```c
#include "types.h"       // 열거형
#include "words/words.h" // WordList, WordCatalog
#include "game.h"         // GameState (types.h, words/words.h 포함)
```
