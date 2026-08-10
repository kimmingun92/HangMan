#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "words/words.h"

#define MAX_WORD_LEN 32
#define MAX_GUESSES 6

typedef struct {
    char word[MAX_WORD_LEN];
    char guessed[MAX_WORD_LEN];
    unsigned char tried[26];
    int wrong_count;
    int game_over;
    int won;
    Difficulty difficulty;
    char category[24];
    const WordList* active_list;
} GameState;

void game_init(GameState* state, Difficulty difficulty);
void game_guess(GameState* state, char letter);
char game_choose_auto_letter(const GameState* state);
int game_letter_used(const GameState* state, char letter);

#endif
