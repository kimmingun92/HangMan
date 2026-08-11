# src/

게임의 구현부(.c) 파일들이 위치합니다. 대응되는 헤더 파일은 `../include/`에 있습니다.

| 파일 | 설명 |
| --- | --- |
| `main.c` | 프로그램 진입점(`main`). raylib 윈도우 초기화, 게임 루프, 화면별 렌더링 함수(`draw_home`, `draw_difficulty`, `draw_play`, `draw_result`), UI 버튼/키보드 입력 처리, 행맨 그림 렌더링을 담당합니다. |
| `game.c` | 게임 진행 로직. 단어 초기화(`game_init`), 글자 추측 처리(`game_guess`), 자동 모드의 글자 선택 알고리즘(`game_choose_auto_letter`, `score_letter`)을 포함합니다. raylib에 의존하지 않는 순수 로직입니다. |
| `words/words.c` | 난이도별 카테고리-단어 카탈로그를 조립하고 `words_get_catalog()`로 제공합니다. |

## 의존 관계

```
main.c ──▶ game.h, types.h
game.c ──▶ game.h, words/words.h
words/words.c ──▶ words/words.h, animal.h, color.h, food.h, fruit.h, tech.h
```
