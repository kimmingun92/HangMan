# src/words/

단어 카탈로그 조립 로직이 위치합니다.

| 파일 | 설명 |
| --- | --- |
| `words.c` | `../../include/words/*.h`에 정의된 카테고리별 단어 배열(animal, color, food, fruit, tech)을 난이도(Easy/Medium/Hard)별 `WordList` 배열로 묶고, `words_get_catalog(Difficulty)` 함수로 외부에 제공합니다. |

실제 단어 데이터(문자열 배열)는 이 폴더가 아니라 `include/words/`의 헤더 파일들에 정의되어 있습니다.
