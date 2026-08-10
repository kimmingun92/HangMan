#ifndef TYPES_H
#define TYPES_H

typedef enum {
    SCREEN_HOME,
    SCREEN_MODE,
    SCREEN_PLAY,
    SCREEN_RESULT
} ScreenState;

typedef enum {
    MODE_MANUAL,
    MODE_AUTO
} GameMode;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;

#endif
