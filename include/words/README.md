# include/words/

단어 카탈로그의 타입 정의와 실제 단어 데이터가 위치합니다.

| 파일 | 설명 |
| --- | --- |
| `words.h` | `WordList`(카테고리 1개 분량의 단어 배열) / `WordCatalog`(난이도 1개 분량의 `WordList` 모음) 구조체와 `words_get_catalog(Difficulty)` 함수 원형을 선언합니다. 실제 조립 로직은 `src/words/words.c`에 있습니다. |
| `animal.h` | 동물 카테고리 단어 (난이도별 `animal_easy_words`, `animal_medium_words`, `animal_hard_words`) |
| `color.h` | 색상 카테고리 단어 |
| `food.h` | 음식 카테고리 단어 |
| `fruit.h` | 과일 카테고리 단어 |
| `tech.h` | IT/기술 카테고리 단어 |

## 단어 추가/수정 방법

1. 해당 카테고리 헤더 파일(예: `fruit.h`)에서 원하는 난이도 배열에 단어를 추가합니다.
2. 배열 크기는 `src/words/words.c`의 `ARRAY_LEN` 매크로가 자동으로 계산하므로 별도 개수 수정은 필요 없습니다.
3. 새 카테고리를 통째로 추가하려면: 새 헤더 파일 생성 → `src/words/words.c`에 `#include` 및 `easy_lists`/`medium_lists`/`hard_lists` 배열에 항목 추가.
