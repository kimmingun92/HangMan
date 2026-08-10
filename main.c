#include "raylib.h"

#if defined(HAVE_RAYGUI)
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "game.h"
#include "types.h"

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 720
#define AUTO_STEP_SECONDS 0.8f

typedef struct {
    ScreenState screen;      // 현재 화면 상태
    GameMode mode;
    Difficulty difficulty;   // 난이도
    GameState game;          // 실제 게임 상태(단어, 정답, 틀린 횟수)
    float auto_elapsed;      // 자동 모드에서 시간 흐름 저장
} AppState;

static const Color BG_TOP = {11, 26, 41, 255};
static const Color BG_BOTTOM = {48, 70, 53, 255};
static const Color PANEL = {20, 35, 45, 235};
static const Color PANEL_ALT = {35, 59, 63, 235};
static const Color ACCENT = {239, 156, 75, 255};
static const Color POSITIVE = {115, 203, 136, 255};
static const Color NEGATIVE = {227, 92, 92, 255};
static const Color INK = {230, 238, 241, 255};

static const char* mode_text(GameMode mode) {
    return mode == MODE_AUTO ? "AUTO" : "MANUAL";
}

static const char* difficulty_text(Difficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY:
            return "EASY";
        case DIFFICULTY_MEDIUM:
            return "MEDIUM";
        case DIFFICULTY_HARD:
        default:
            return "HARD";
    }
}

// 새로운 게임 시작 시 실행되는 함수
static void begin_round(AppState* app) {
    game_init(&app->game, app->difficulty); // 단어 초기화, 게임 상태 초기화
    app->screen = SCREEN_PLAY;              // 게임 화면으로 전환
    app->auto_elapsed = 0.0f;               // 자동 모드 타이머 초기화
}

static void draw_title(const char* text, int y, int size, Color color) {
    int w = MeasureText(text, size);
    int x = (WINDOW_WIDTH - w) / 2;
    // 텍스트 그림자로 가독성 향상
    DrawText(text, x + 2, y + 2, size, (Color){0, 0, 0, 80});
    DrawText(text, x, y, size, color);
}

static void draw_background(void) {
    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BG_TOP, BG_BOTTOM);
    DrawCircle(980, 60, 150.0f, (Color){255, 255, 255, 14});
    DrawCircle(110, 670, 180.0f, (Color){255, 255, 255, 10});
}

static void draw_background_danger(int intensity) {
    // 위험도에 따라 배경이 점점 어두워지고 붉어짐
    float danger_factor = intensity / 6.0f;
    Color top = {
        (unsigned char)(11 + 30 * danger_factor),
        (unsigned char)(26 - 10 * danger_factor),
        (unsigned char)(41 - 20 * danger_factor),
        255
    };
    Color bottom = {
        (unsigned char)(48 + 20 * danger_factor),
        (unsigned char)(70 - 30 * danger_factor),
        (unsigned char)(53 - 20 * danger_factor),
        255
    };

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, top, bottom);

    // 위험 신호 효과
    if (intensity >= 4) {
        float pulse = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
        DrawCircle(980, 60, 150.0f + pulse * 20.0f, (Color){255, 100, 100, (unsigned char)(20 + pulse * 15)});
    } else {
        DrawCircle(980, 60, 150.0f, (Color){255, 255, 255, 14});
    }

    DrawCircle(110, 670, 180.0f, (Color){255, 255, 255, 10});
}

static void draw_background_victory(void) {
    // 승리 배경 - 밝고 긍정적인 색상
    Color top = {20, 60, 80, 255};
    Color bottom = {80, 130, 100, 255};
    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, top, bottom);

    // 승리 파티클 효과
    float time = GetTime();
    for (int i = 0; i < 12; i++) {
        float offset = i * 0.5f;
        float x = 200.0f + sinf(time * 1.5f + offset) * 150.0f;
        float y = 100.0f + i * 50.0f + cosf(time * 2.0f + offset) * 30.0f;
        float pulse = (sinf(time * 3.0f + offset) + 1.0f) / 2.0f;
        DrawCircle((int)x, (int)y, 8.0f + pulse * 5.0f, (Color){255, 220, 100, (unsigned char)(80 + pulse * 60)});
    }

    DrawCircle(980, 60, 150.0f, (Color){255, 255, 100, 25});
    DrawCircle(110, 670, 180.0f, (Color){100, 255, 200, 15});
}

static int ui_button(Rectangle rect, const char* label) {
#if defined(HAVE_RAYGUI)
    return GuiButton(rect, label);
#else
    Vector2 mouse = GetMousePosition();
    int hover = CheckCollisionPointRec(mouse, rect);
    Color fill = hover ? ACCENT : PANEL_ALT;
    DrawRectangleRounded(rect, 0.25f, 8, fill);
    DrawRectangleRoundedLines(rect, 0.25f, 8, INK);
    int text_size = 24;
    int text_w = MeasureText(label, text_size);
    DrawText(label, (int)(rect.x + (rect.width - text_w) / 2), (int)(rect.y + 13), text_size, INK);
    return hover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
#endif
}

static void draw_hangman(int wrong_count) {
    Color stick_color = INK;

    // 위험도에 따라 색상 변경
    if (wrong_count >= 5) {
        stick_color = NEGATIVE; // 빨간색
    } else if (wrong_count >= 3) {
        stick_color = (Color){255, 180, 100, 255}; // 주황색
    }

    // 교수대
    DrawLineEx((Vector2){90, 560}, (Vector2){90, 110}, 6.0f, stick_color);
    DrawLineEx((Vector2){90, 110}, (Vector2){250, 110}, 6.0f, stick_color);
    DrawLineEx((Vector2){250, 110}, (Vector2){250, 150}, 6.0f, stick_color);

    // 머리 (1번 실수)
    if (wrong_count >= 1) {
        DrawCircle(250, 185, 35, (Color){255, 220, 177, 255}); // 살색
        DrawCircleLines(250, 185, 35.0f, stick_color);

        // 표정 추가
        if (wrong_count >= 6) {
            // 죽은 표정 (X_X)
            DrawLineEx((Vector2){236, 178}, (Vector2){242, 184}, 3.0f, RED);
            DrawLineEx((Vector2){242, 178}, (Vector2){236, 184}, 3.0f, RED);
            DrawLineEx((Vector2){258, 178}, (Vector2){264, 184}, 3.0f, RED);
            DrawLineEx((Vector2){264, 178}, (Vector2){258, 184}, 3.0f, RED);
            DrawCircle(250, 197, 6, RED); // 입 (울음)
        } else if (wrong_count >= 4) {
            // 무서운 표정 (O_O)
            DrawCircle(240, 180, 5, (Color){50, 50, 80, 255});
            DrawCircle(260, 180, 5, (Color){50, 50, 80, 255});
            DrawCircle(240, 180, 2, INK);
            DrawCircle(260, 180, 2, INK);
            DrawCircle(250, 197, 8, (Color){100, 80, 80, 255}); // 입 (공포)
        } else if (wrong_count >= 2) {
            // 걱정하는 표정 (눈썹 처짐)
            DrawLineEx((Vector2){235, 175}, (Vector2){242, 178}, 2.0f, (Color){80, 60, 40, 255});
            DrawLineEx((Vector2){258, 178}, (Vector2){265, 175}, 2.0f, (Color){80, 60, 40, 255});
            DrawCircle(240, 182, 3, (Color){50, 50, 80, 255});
            DrawCircle(260, 182, 3, (Color){50, 50, 80, 255});
            DrawLineEx((Vector2){242, 195}, (Vector2){258, 197}, 2.0f, (Color){120, 80, 80, 255}); // 입 (걱정)
        } else {
            // 웃는 표정 (^_^)
            DrawCircle(240, 180, 4, (Color){50, 50, 80, 255});
            DrawCircle(260, 180, 4, (Color){50, 50, 80, 255});
            DrawCircleLines(250, 192, 8, (Color){200, 100, 100, 255}); // 웃는 입
            DrawCircleSector((Vector2){250, 192}, 8, 180, 360, 8, (Color){200, 100, 100, 255});
        }
    }

    // 몸통 (2번 실수)
    if (wrong_count >= 2) {
        DrawLineEx((Vector2){250, 220}, (Vector2){250, 345}, 5.0f, stick_color);
    }

    // 왼팔 (3번 실수)
    if (wrong_count >= 3) {
        DrawLineEx((Vector2){250, 255}, (Vector2){190, 305}, 5.0f, stick_color);
    }

    // 오른팔 (4번 실수)
    if (wrong_count >= 4) {
        DrawLineEx((Vector2){250, 255}, (Vector2){310, 305}, 5.0f, stick_color);
    }

    // 왼다리 (5번 실수)
    if (wrong_count >= 5) {
        DrawLineEx((Vector2){250, 345}, (Vector2){205, 420}, 5.0f, stick_color);
    }

    // 오른다리 (6번 실수)
    if (wrong_count >= 6) {
        DrawLineEx((Vector2){250, 345}, (Vector2){295, 420}, 5.0f, stick_color);
    }
}

static void build_spaced_word(const GameState* game, char* out, int out_size) {
    int index = 0;
    for (int i = 0; game->guessed[i] != '\0' && index < out_size - 3; i++) {
        out[index++] = (char)toupper((unsigned char)game->guessed[i]);
        out[index++] = ' ';
    }

    if (index > 0) {
        index--;
    }
    out[index] = '\0';
}

static void draw_home(AppState* app) {
    draw_title("HANGMAN CONTROL ROOM", 96, 56, INK);
    draw_title("raylib + raygui refactor", 162, 26, (Color){200, 219, 215, 255});

    DrawRectangleRounded((Rectangle){340, 220, 420, 320}, 0.08f, 8, PANEL);
    DrawRectangleRoundedLines((Rectangle){340, 220, 420, 320}, 0.08f, 8, ACCENT);

    // 메인 화면에서 버튼 클릭 시 상태 변경
    if (ui_button((Rectangle){390, 280, 320, 58}, "MANUAL START")) {
        app->mode = MODE_MANUAL;
        app->screen = SCREEN_MODE; // 난이도 선택 화면으로 이동
    }
    if (ui_button((Rectangle){390, 355, 320, 58}, "AUTO START")) {
        app->mode = MODE_AUTO;
        app->screen = SCREEN_MODE;
    }
    if (ui_button((Rectangle){390, 430, 320, 58}, "EXIT")) {
        CloseWindow(); // 프로그램 종료
    }
}

// 난이도 선택
static void draw_difficulty(AppState* app) {
    char subtitle[120];
    snprintf(subtitle, sizeof(subtitle), "Mode: %s. Choose your word pool.", mode_text(app->mode));

    draw_title("DIFFICULTY SELECT", 96, 50, INK);
    draw_title(subtitle, 158, 24, (Color){204, 219, 215, 255});

    DrawRectangleRounded((Rectangle){275, 220, 550, 360}, 0.08f, 10, PANEL);
    DrawRectangleRoundedLines((Rectangle){275, 220, 550, 360}, 0.08f, 10, ACCENT);

    if (ui_button((Rectangle){355, 270, 390, 58}, "EASY")) {
        app->difficulty = DIFFICULTY_EASY; // 난이도 저장
        begin_round(app);                  // 게임 시작 함수 호출
    }
    if (ui_button((Rectangle){355, 345, 390, 58}, "MEDIUM")) {
        app->difficulty = DIFFICULTY_MEDIUM;
        begin_round(app);
    }
    if (ui_button((Rectangle){355, 420, 390, 58}, "HARD")) {
        app->difficulty = DIFFICULTY_HARD;
        begin_round(app);
    }
    if (ui_button((Rectangle){355, 495, 390, 50}, "BACK")) {
        app->screen = SCREEN_HOME;
    }
}

static void draw_letter_grid(AppState* app) {
    // QWERTY 키보드 레이아웃
    const char* rows[3] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    const int row_offsets[3] = {0, 30, 60}; // 각 행의 시작 오프셋

    const int button_w = 48;
    const int button_h = 48;
    const int gap = 8;
    const int start_x = 470;
    const int start_y = 470;

    for (int row = 0; row < 3; row++) {
        const char* letters = rows[row];
        int len = (int)strlen(letters);

        for (int col = 0; col < len; col++) {
            char letter = letters[col];
            char label[2] = {letter, '\0'};
            int letter_index = tolower((unsigned char)letter) - 'a';

            Rectangle rect = {
                (float)(start_x + row_offsets[row] + col * (button_w + gap)),
                (float)(start_y + row * (button_h + gap)),
                (float)button_w,
                (float)button_h
            };

            if (app->game.tried[letter_index]) {
                DrawRectangleRounded(rect, 0.25f, 6, (Color){65, 75, 78, 255});
                int text_w = MeasureText(label, 24);
                DrawText(label, (int)rect.x + (button_w - text_w) / 2, (int)rect.y + 12, 24, (Color){150, 160, 162, 255});
                continue;
            }

            if (ui_button(rect, label)) {
                game_guess(&app->game, (char)tolower((unsigned char)letter));
            }
        }
    }
}

static void update_manual_input(AppState* app) {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
            game_guess(&app->game, (char)key);
        }
        key = GetCharPressed();
    }
}

static void update_auto_mode(AppState* app) {
    app->auto_elapsed += GetFrameTime();
    if (app->auto_elapsed < AUTO_STEP_SECONDS) {
        return;
    }

    app->auto_elapsed = 0.0f;
    char next = game_choose_auto_letter(&app->game);
    if (next != '\0') {
        game_guess(&app->game, next);
    }
}

static void draw_play(AppState* app) {
    char word_view[96] = {0};
    char stats_text[128] = {0};

    // 게임 상태에 따라 다른 배경
    if (app->game.wrong_count > 0) {
        draw_background_danger(app->game.wrong_count);
    } else {
        draw_background();
    }

    DrawRectangleRounded((Rectangle){36, 36, 1028, 648}, 0.04f, 8, PANEL);
    DrawRectangleRoundedLines((Rectangle){36, 36, 1028, 648}, 0.04f, 8, ACCENT);

    DrawRectangleRounded((Rectangle){420, 80, 590, 350}, 0.05f, 8, PANEL_ALT);

    // 단두대 영역 배경 색상 (실패 횟수에 따라 점진적 위험 강조)
    float danger_factor = (float)app->game.wrong_count / (float)MAX_GUESSES;
    if (danger_factor > 1.0f) danger_factor = 1.0f;
    Color gallows_bg = {
        (unsigned char)((26 * (1.0f - danger_factor)) + (150 * danger_factor)),
        (unsigned char)((43 * (1.0f - danger_factor)) + (40 * danger_factor)),
        (unsigned char)((57 * (1.0f - danger_factor)) + (45 * danger_factor)),
        255
    };
    DrawRectangleRounded((Rectangle){72, 80, 320, 560}, 0.05f, 8, gallows_bg);

    draw_hangman(app->game.wrong_count);
    build_spaced_word(&app->game, word_view, (int)sizeof(word_view));

    // 라벨과 단어를 더 깔끔하게 표시
    DrawText("WORD", 460, 120, 32, (Color){180, 200, 195, 255});

    // 단어 길이에 따라 폰트 크기 자동 조정
    int base_font_size = 64;
    int max_width = 520; // 사용 가능한 최대 너비
    int text_width = MeasureText(word_view, base_font_size);

    int word_font_size = base_font_size;
    if (text_width > max_width) {
        word_font_size = (base_font_size * max_width) / text_width;
        if (word_font_size < 32) word_font_size = 32; // 최소 크기
    }

    DrawText(word_view, 460, 165, word_font_size, INK);

    // 게임 정보를 더 깔끔하게 표시
    snprintf(stats_text, sizeof(stats_text), "CATEGORY %s", app->game.category);
    DrawText(stats_text, 460, 260, 28, POSITIVE);
    snprintf(stats_text, sizeof(stats_text), "MODE %s", mode_text(app->mode));
    DrawText(stats_text, 460, 300, 28, (Color){200, 220, 230, 255});
    snprintf(stats_text, sizeof(stats_text), "LEVEL %s", difficulty_text(app->difficulty));
    DrawText(stats_text, 460, 340, 28, (Color){200, 220, 230, 255});
    snprintf(stats_text, sizeof(stats_text), "WRONG %d / %d", app->game.wrong_count, MAX_GUESSES);
    DrawText(stats_text, 460, 380, 32, NEGATIVE);

    if (ui_button((Rectangle){72, 595, 135, 38}, "MENU")) {
        app->screen = SCREEN_HOME;
    }

    if (app->mode == MODE_MANUAL) {
        DrawText("CLICK LETTERS OR TYPE A-Z", 455, 435, 24, (Color){198, 211, 207, 255});
        draw_letter_grid(app);
    } else {
        DrawRectangleRounded((Rectangle){455, 470, 530, 110}, 0.12f, 8, (Color){44, 78, 56, 240});
        DrawText("AUTO MODE IS GUESSING EVERY 0.8s", 488, 508, 27, INK);
        DrawText("rule-based guessing only", 635, 542, 21, (Color){205, 228, 214, 255});
        update_auto_mode(app);
    }

    if (!app->game.game_over && app->mode == MODE_MANUAL) {
        update_manual_input(app);
    }

    if (app->game.game_over) {
        app->screen = SCREEN_RESULT;
    }
}

static void draw_result(AppState* app) {
    char answer[96];
    snprintf(answer, sizeof(answer), "ANSWER: %s", app->game.word);

    // 승리 시 특별한 배경
    if (app->game.won) {
        draw_background_victory();
    } else {
        draw_background_danger(6); // 패배 시 최대 위험 배경
    }

    DrawRectangleRounded((Rectangle){36, 36, 1028, 648}, 0.04f, 8, PANEL);
    DrawRectangleRoundedLines((Rectangle){36, 36, 1028, 648}, 0.04f, 8, app->game.won ? POSITIVE : NEGATIVE);

    DrawRectangleRounded((Rectangle){420, 80, 590, 350}, 0.05f, 8, PANEL_ALT);
    DrawRectangleRounded((Rectangle){72, 80, 320, 560}, 0.05f, 8, (Color){26, 43, 57, 255});

    draw_hangman(app->game.wrong_count);

    char word_view[96] = {0};
    build_spaced_word(&app->game, word_view, (int)sizeof(word_view));

    // 라벨과 단어를 더 깔끔하게 표시
    DrawText("WORD", 460, 120, 32, (Color){180, 200, 195, 255});

    // 단어 길이에 따라 폰트 크기 자동 조정
    int base_font_size = 64;
    int max_width = 520; // 사용 가능한 최대 너비
    int text_width = MeasureText(word_view, base_font_size);

    int word_font_size = base_font_size;
    if (text_width > max_width) {
        word_font_size = (base_font_size * max_width) / text_width;
        if (word_font_size < 32) word_font_size = 32; // 최소 크기
    }

    DrawText(word_view, 460, 165, word_font_size, INK);

    char stats_text[128] = {0};
    // 게임 정보를 더 깔끔하게 표시
    snprintf(stats_text, sizeof(stats_text), "CATEGORY %s", app->game.category);
    DrawText(stats_text, 460, 260, 28, POSITIVE);
    snprintf(stats_text, sizeof(stats_text), "MODE %s", mode_text(app->mode));
    DrawText(stats_text, 460, 300, 28, (Color){200, 220, 230, 255});
    snprintf(stats_text, sizeof(stats_text), "LEVEL %s", difficulty_text(app->difficulty));
    DrawText(stats_text, 460, 340, 28, (Color){200, 220, 230, 255});
    snprintf(stats_text, sizeof(stats_text), "WRONG %d / %d", app->game.wrong_count, MAX_GUESSES);
    DrawText(stats_text, 460, 380, 32, NEGATIVE);

    // 결과 창 표시
    DrawRectangleRounded((Rectangle){300, 220, 500, 280}, 0.08f, 8, (Color){13, 19, 25, 250});
    DrawRectangleRoundedLines((Rectangle){300, 220, 500, 280}, 0.08f, 8, app->game.won ? POSITIVE : NEGATIVE);

    // 결과 텍스트
    const char* title = app->game.won ? "VICTORY!" : "GAME OVER";
    int title_size = 54;
    int title_w = MeasureText(title, title_size);
    int title_x = (WINDOW_WIDTH - title_w) / 2;
    DrawText(title, title_x + 3, 253, title_size, (Color){0, 0, 0, 100});
    DrawText(title, title_x, 250, title_size, app->game.won ? POSITIVE : NEGATIVE);

    // 정답 표시
    int answer_size = 28;
    int answer_w = MeasureText(answer, answer_size);
    int answer_x = (WINDOW_WIDTH - answer_w) / 2;
    DrawText(answer, answer_x, 330, answer_size, INK);

    // 승리 시 축하 메시지
    if (app->game.won) {
        const char* msg = "Excellent work!";
        int msg_w = MeasureText(msg, 24);
        DrawText(msg, (WINDOW_WIDTH - msg_w) / 2, 375, 24, (Color){180, 230, 190, 255});
    }

    if (ui_button((Rectangle){360, 430, 180, 50}, "RESTART")) {
        begin_round(app);
    }
    if (ui_button((Rectangle){560, 430, 180, 50}, "HOME")) {
        app->screen = SCREEN_HOME;
    }
}

int main(void) {
    AppState app; // 프로그램 전체 상태를 저장할 구조체 변수

    srand((unsigned int)time(NULL));

    app.screen = SCREEN_HOME;
    app.mode = MODE_MANUAL;
    app.difficulty = DIFFICULTY_EASY;
    app.auto_elapsed = 0.0f;
    memset(&app.game, 0, sizeof(app.game));

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hangman - raylib");
    SetTargetFPS(60);

    // 게임 루프
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE) && app.screen != SCREEN_PLAY) {
            app.screen = SCREEN_HOME;
        }

        BeginDrawing();       // 한 프레임 그리기
        draw_background();    // 기본 배경 출력

        // 현재 화면 상태에 따라 함수 호출
        switch (app.screen) {
            case SCREEN_HOME:
                draw_home(&app);
                break;
            case SCREEN_MODE:
                draw_difficulty(&app);
                break;
            case SCREEN_PLAY:
                draw_play(&app);
                break;
            case SCREEN_RESULT:
                draw_result(&app);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
