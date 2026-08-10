#include "game.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    Difficulty difficulty;
    const char* category;
    int length;
    const char* order;
} CategoryLengthOrder;

static const int global_letter_score[26] = {
    82, 15, 28, 43, 127, 22, 20, 61, 70, 2, 8, 40, 24,
    67, 75, 19, 1, 60, 63, 91, 28, 10, 24, 2, 20, 1
};

static const char* high_presence_order = "eaionrstludcmpghbyfvwkzxjq";

static const CategoryLengthOrder category_length_orders[] = {
    {DIFFICULTY_EASY, "Fruit", 4, "elmpairubcdfghjknoqstvwxyz"},
    {DIFFICULTY_EASY, "Fruit", 5, "ealogmnprvbchiuydfjkqstwxz"},
    {DIFFICULTY_EASY, "Animal", 5, "eaorshglkptbdimnuwzcfjqvxy"},
    {DIFFICULTY_EASY, "Color", 3, "derabcfghijklmnopqstuvwxyz"},
    {DIFFICULTY_EASY, "Color", 4, "alnyegbcdikoprtuvfhjmqswxz"},
    {DIFFICULTY_EASY, "Color", 5, "benrwacghiklotdfjmpqsuvxyz"},
    {DIFFICULTY_EASY, "Food", 4, "oacpstubdefghijklmnqrvwxyz"},
    {DIFFICULTY_EASY, "Food", 5, "astbcdeiopruhklnyzfgjmqvwx"},
    {DIFFICULTY_EASY, "Food", 6, "ceikoabdfghjlmnpqrstuvwxyz"},
    {DIFFICULTY_EASY, "Tech", 5, "elabcimopsuxdfghjknqrtvwyz"},
    {DIFFICULTY_EASY, "Tech", 6, "eranstbcgilodmuvyfhjkpqwxz"},

    {DIFFICULTY_MEDIUM, "Fruit", 6, "anreoilmptuybcdghswfjkqvxz"},
    {DIFFICULTY_MEDIUM, "Fruit", 7, "coatdinpruvbefghjklmqswxyz"},
    {DIFFICULTY_MEDIUM, "Animal", 6, "raeodltbiknuycgpsfmwzhjqvx"},
    {DIFFICULTY_MEDIUM, "Color", 5, "aibceghklordfjmnpqstuvwxyz"},
    {DIFFICULTY_MEDIUM, "Color", 6, "oelnraigmsvdptuwybcfhjkqxz"},
    {DIFFICULTY_MEDIUM, "Color", 7, "acelrstbdfghijkmnopquvwxyz"},
    {DIFFICULTY_MEDIUM, "Food", 6, "elobdgmnrtuacfhijkpqsvwxyz"},
    {DIFFICULTY_MEDIUM, "Food", 7, "asencibglpuwfhkortdjmqvxyz"},
    {DIFFICULTY_MEDIUM, "Food", 8, "dinacghlmpsuwbefjkoqrtvxyz"},
    {DIFFICULTY_MEDIUM, "Tech", 7, "retaoidgknpswcbhlmuyfjqvxz"},
    {DIFFICULTY_MEDIUM, "Tech", 8, "aefilrwbcdghjkmnopqstuvxyz"},

    {DIFFICULTY_HARD, "Fruit", 9, "enairpbcglmostydfhjkquvwxz"},
    {DIFFICULTY_HARD, "Fruit", 10, "aelrtbcnowykmpsudfghijqvxz"},
    {DIFFICULTY_HARD, "Fruit", 11, "noraegtbdfimpsuychjklqvwxz"},
    {DIFFICULTY_HARD, "Fruit", 12, "afinoprstubcdeghjklmqvwxyz"},
    {DIFFICULTY_HARD, "Animal", 8, "goadehknrbcfijlmpqstuvwxyz"},
    {DIFFICULTY_HARD, "Animal", 9, "orailgndeftuychjmpsbkqvwxz"},
    {DIFFICULTY_HARD, "Animal", 10, "cehinopradkmswzbfgjlqtuvxy"},
    {DIFFICULTY_HARD, "Animal", 11, "aceilprtbdfghjkmnoqsuvwxyz"},
    {DIFFICULTY_HARD, "Color", 5, "aeipsbcdfghjklmnoqrtuvwxyz"},
    {DIFFICULTY_HARD, "Color", 7, "anefgmorstbcdhijklpquvwxyz"},
    {DIFFICULTY_HARD, "Color", 8, "nradelubcgvyfhijkmopqstwxz"},
    {DIFFICULTY_HARD, "Color", 9, "eiorlmnqstuvabcdfghjkpwxyz"},
    {DIFFICULTY_HARD, "Color", 10, "erainuchklmpqstwbdfgjovxyz"},
    {DIFFICULTY_HARD, "Color", 11, "aeilmnrtubcdfghjkopqsvwxyz"},
    {DIFFICULTY_HARD, "Food", 8, "abimpcdefghjklnoqrstuvwxyz"},
    {DIFFICULTY_HARD, "Food", 9, "acobegilmnrsthjpuydfkqvwxz"},
    {DIFFICULTY_HARD, "Food", 10, "aesuchikrtbdlnopqfgjmvwxyz"},
    {DIFFICULTY_HARD, "Food", 11, "aeilortubcdfghjkmnpqsvwxyz"},
    {DIFFICULTY_HARD, "Tech", 9, "hiotablmrcdegnpuwfjkqsvxyz"},
    {DIFFICULTY_HARD, "Tech", 10, "iotenpryacmsubdfghjklqvwxz"},
    {DIFFICULTY_HARD, "Tech", 11, "aderthilmubovzcfgjknpqswxy"},
    {DIFFICULTY_HARD, "Tech", 13, "cirtuabefgnosydhjklmpqvwxz"}
};

static int is_vowel(char letter) {
    return letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u';
}

static int is_rare_letter(char letter) {
    return letter == 'j' || letter == 'q' || letter == 'x' || letter == 'z' || letter == 'v' || letter == 'k';
}

static int order_rank(const char* order, char letter) {
    for (int i = 0; order[i] != '\0'; i++) {
        if (order[i] == letter) {
            return i;
        }
    }
    return 26;
}

static int count_hidden_letters(const GameState* state) {
    int hidden = 0;

    for (int i = 0; state->guessed[i] != '\0'; i++) {
        if (state->guessed[i] == '_') {
            hidden++;
        }
    }

    return hidden;
}

static int count_revealed_vowels(const GameState* state) {
    int count = 0;

    for (int i = 0; state->guessed[i] != '\0'; i++) {
        char letter = (char)tolower((unsigned char)state->guessed[i]);
        if (is_vowel(letter)) {
            count++;
        }
    }

    return count;
}

static int count_revealed_consonants(const GameState* state) {
    int count = 0;

    for (int i = 0; state->guessed[i] != '\0'; i++) {
        char letter = (char)tolower((unsigned char)state->guessed[i]);
        if (letter >= 'a' && letter <= 'z' && !is_vowel(letter)) {
            count++;
        }
    }

    return count;
}

static const char* lookup_category_order(const GameState* state) {
    int target_len = (int)strlen(state->word);
    int best_gap_same_diff = 9999;
    int best_gap_any_diff = 9999;
    const char* best_order_same_diff = NULL;
    const char* best_order = NULL;

    for (int i = 0; i < (int)(sizeof(category_length_orders) / sizeof(category_length_orders[0])); i++) {
        const CategoryLengthOrder* row = &category_length_orders[i];
        int gap;

        if (strcmp(row->category, state->category) != 0) {
            continue;
        }

        if (row->difficulty == state->difficulty && row->length == target_len) {
            return row->order;
        }

        gap = row->length - target_len;
        if (gap < 0) {
            gap = -gap;
        }

        if (row->difficulty == state->difficulty && gap < best_gap_same_diff) {
            best_gap_same_diff = gap;
            best_order_same_diff = row->order;
        } else if (gap < best_gap_any_diff) {
            best_gap_any_diff = gap;
            best_order = row->order;
        }
    }

    if (best_order_same_diff) {
        return best_order_same_diff;
    }

    if (best_order) {
        return best_order;
    }

    return high_presence_order;
}

// 글자 하나가 얼마나 유리한 선택인지 점수로 계산하는 함수
static int score_letter(const GameState* state, char letter) {
    // 현재 카테고리 + 단어 길이에 맞는 글자 우선순위 가져오기
    const char* category_order = lookup_category_order(state);
    // 전체 영어 빈도 기준 순위
    int fallback_rank = order_rank(high_presence_order, letter);
    // 현재 카테고리 기준에서의 순위
    int category_rank = order_rank(category_order, letter);
    int word_len = (int)strlen(state->word);
    // 아직 공개되지 않은 글자 개수
    int hidden_count = count_hidden_letters(state);
    // 현재 공개된 모음 개수
    int vowel_count = count_revealed_vowels(state);
    // 현재 공개된 자음 개수
    int consonant_count = count_revealed_consonants(state);
    // 남은 기회 수
    int remaining_lives = MAX_GUESSES - state->wrong_count;
    // 최종 점수
    int score = 0;
    // 알파벳 인덱스 (a=0, b=1, ...)
    int idx = letter - 'a';

    // 카테고리 기반 중요도 (가장 영향 큼)
    score += (26 - category_rank) * 100;
    // 전체 영어 빈도 (보조 기준)
    score += (26 - fallback_rank) * 2;
    // 기본 글자 빈도 점수
    score += global_letter_score[idx] / 4;

    // 모음/자음 균형 조정
    if (is_vowel(letter)) {
        // 초반: 모음이 하나도 없으면 우선 선택
        if (vowel_count == 0) {
            score += 24;
        }
        // 중후반: 모음이 많으면 감점
        else if (vowel_count >= consonant_count && hidden_count <= word_len / 2) {
            score -= 10;
        }
    } else {
        // 초반: 모음 없으면 자음은 불리
        if (vowel_count == 0) {
            score -= 6;
        }
        // 자음이 부족하면 가산점
        else if (consonant_count < vowel_count) {
            score += 4;
        }
    }

    // 남은 목숨 기반 위험 제어 (희귀 문자 감점)
    if (remaining_lives <= 2 && is_rare_letter(letter)) {
        score -= 25; // 매우 위험
    } else if (remaining_lives <= 3 && is_rare_letter(letter)) {
        score -= 15;
    } else if (remaining_lives <= 4 && is_rare_letter(letter)) {
        score -= 8;
    }

    // 최종 점수 반환
    return score;
}

void game_init(GameState* state, Difficulty difficulty) {
    // 카테고리를 랜덤 선택하고 그 안에서 단어 하나를 랜덤으로 선택합니다
    WordCatalog catalog = words_get_catalog(difficulty);
    int list_index = rand() % catalog.count;
    const WordList* selected_list = &catalog.lists[list_index];
    int word_index = rand() % selected_list->count;

    snprintf(state->word, sizeof(state->word), "%s", selected_list->words[word_index]);
    snprintf(state->category, sizeof(state->category), "%s", selected_list->category);
    state->difficulty = difficulty;
    state->active_list = selected_list;

    // 여기서는 플레이어에게 보여줄 단어 상태를 초기화합니다.
    memset(state->guessed, 0, sizeof(state->guessed));
    for (int i = 0; state->word[i] != '\0'; i++) {
        state->guessed[i] = '_';
    }

    memset(state->tried, 0, sizeof(state->tried));
    state->wrong_count = 0;
    state->game_over = 0;
    state->won = 0;
}

void game_guess(GameState* state, char letter) {
    letter = (char)tolower((unsigned char)letter);
    if (letter < 'a' || letter > 'z') {
        return;
    }

    if (state->tried[letter - 'a']) {
        return;
    }

    state->tried[letter - 'a'] = 1;

    // 사용자가 글자를 입력하면 이 함수에서 정답 여부를 판단합니다.
    // 맞으면 해당 위치 공개
    // 틀리면 wrong_count 증가
    int found = 0;
    for (int i = 0; state->word[i]; i++) {
        if (tolower((unsigned char)state->word[i]) == letter) {
            state->guessed[i] = state->word[i];
            found = 1;
        }
    }

    if (!found) {
        state->wrong_count++;
    }

    if (state->wrong_count >= MAX_GUESSES) {
        state->game_over = 1; // 패배
    } else if (strcmp(state->word, state->guessed) == 0) {
        state->game_over = 1;
        state->won = 1; // 승리
    }
}

char game_choose_auto_letter(const GameState* state) {
    int best_score = -1000000;
    int best_global_rank = 9999;
    char best_letter = '\0';

    for (int i = 0; i < 26; i++) {
        char letter = (char)('a' + i);
        int score;
        int rank;

        if (state->tried[i]) {
            continue;
        }

        score = score_letter(state, letter); // 현재 글자가 얼마나 유리한지 점수 계산
        rank = order_rank(high_presence_order, letter); // 전역 빈도 순위 계산(동점처리)

        if (score > best_score || (score == best_score && rank < best_global_rank)) {
            best_score = score;
            best_global_rank = rank;
            best_letter = letter;
        }
    }

    return best_letter;
}

int game_letter_used(const GameState* state, char letter) {
    char normalized = (char)tolower((unsigned char)letter);
    if (normalized < 'a' || normalized > 'z') {
        return 1;
    }
    return state->tried[normalized - 'a'] != 0;
}
