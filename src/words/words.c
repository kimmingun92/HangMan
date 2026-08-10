#include "words/words.h"

#include "words/animal.h"
#include "words/color.h"
#include "words/food.h"
#include "words/fruit.h"
#include "words/tech.h"

#define ARRAY_LEN(items) ((int)(sizeof(items) / sizeof((items)[0])))

static const WordList easy_lists[] = {
    {"Fruit", fruit_easy_words, ARRAY_LEN(fruit_easy_words)},
    {"Animal", animal_easy_words, ARRAY_LEN(animal_easy_words)},
    {"Color", color_easy_words, ARRAY_LEN(color_easy_words)},
    {"Food", food_easy_words, ARRAY_LEN(food_easy_words)},
    {"Tech", tech_easy_words, ARRAY_LEN(tech_easy_words)}
};

static const WordList medium_lists[] = {
    {"Fruit", fruit_medium_words, ARRAY_LEN(fruit_medium_words)},
    {"Animal", animal_medium_words, ARRAY_LEN(animal_medium_words)},
    {"Color", color_medium_words, ARRAY_LEN(color_medium_words)},
    {"Food", food_medium_words, ARRAY_LEN(food_medium_words)},
    {"Tech", tech_medium_words, ARRAY_LEN(tech_medium_words)}
};

static const WordList hard_lists[] = {
    {"Fruit", fruit_hard_words, ARRAY_LEN(fruit_hard_words)},
    {"Animal", animal_hard_words, ARRAY_LEN(animal_hard_words)},
    {"Color", color_hard_words, ARRAY_LEN(color_hard_words)},
    {"Food", food_hard_words, ARRAY_LEN(food_hard_words)},
    {"Tech", tech_hard_words, ARRAY_LEN(tech_hard_words)}
};

WordCatalog words_get_catalog(Difficulty difficulty) {
    WordCatalog catalog;

    switch (difficulty) {
        case DIFFICULTY_EASY:
            catalog.lists = easy_lists;
            catalog.count = ARRAY_LEN(easy_lists);
            break;
        case DIFFICULTY_MEDIUM:
            catalog.lists = medium_lists;
            catalog.count = ARRAY_LEN(medium_lists);
            break;
        case DIFFICULTY_HARD:
        default:
            catalog.lists = hard_lists;
            catalog.count = ARRAY_LEN(hard_lists);
            break;
    }

    return catalog;
}
