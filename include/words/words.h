#ifndef WORDS_WORDS_H
#define WORDS_WORDS_H

#include "types.h"

typedef struct {
    const char* category;
    const char* const* words;
    int count;
} WordList;

typedef struct {
    const WordList* lists;
    int count;
} WordCatalog;

WordCatalog words_get_catalog(Difficulty difficulty);

#endif
