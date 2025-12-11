#include "raylib.h"
#include "raygui.h"
#include "tetris.h"

static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 400;
static TetrisState state = MENU;
static int aniState = 0; // TODO: animation state

static int board[TETRIS_BOARD_H][TETRIS_BOARD_W]; // 0=空，其它代表方塊種類
static int score = 0;
static int level = 1;
static bool gameOver = false;
static bool pause = false;

static const Vector2 SHAPES[7][4][4] = {
    // PIECE_I
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },  // rot 0  ----
        { { 1, -1 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },  // rot 1  
        { { -1, 1 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },  // rot 2
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },  // rot 3
    },
    // PIECE_O
    {
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
    },
    // PIECE_T
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 0 } },
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, 0 } },
    },
    // PIECE_S
    {
        { { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 } },
        { { 0, -1 }, { 0, 0 }, { 1, 0 }, { 1, 1 } },
        { { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 } },
        { { 0, -1 }, { 0, 0 }, { 1, 0 }, { 1, 1 } },
    },
    // PIECE_Z
    {
        { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
        { { 1, -1 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
        { { 1, -1 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
    },
    // PIECE_J
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, -1 } },
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, 1 } },
    },
    // PIECE_L
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
        { { -1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } },
        { { -1, -1 }, { 0, -1 }, { 0, 0 }, { 0, 1 } },
    },
};


void tetris(menuState *mainState) {
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    SetWindowPosition(180, 100);
    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        fixWindowDPI(WINDOW_WIDTH, WINDOW_HEIGHT);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        switch(state) {
        case MENU:
            if (GuiButton((Rectangle) { 80, 90, 120, 30 }, "#191#Start Game"))
                Tetris_Init();
                state = PLAYING;
            if (GuiButton((Rectangle) { 80, 50, 120, 30 }, "#191#Back to Menu"))
                *mainState = MAIN_MENU;
            break;
        case PLAYING:
            Draw_UI();
            Draw_Board();

            if (!gameOver) {
                if (!pause) Tetris_Update(Tetris_GetInput());
                else Draw_PauseScreen();
            } else {
                // TODO: Animation
                state = GAMEOVER;
            }
            break;
        case GAMEOVER:
            Draw_UI();
            Draw_Board();
            // TODO: Game Over Screen
            break;
        }
        
        EndDrawing();
    }
}


// TODO: Implement the functions below
static void Tetris_Init() {
    score = 0;
    level = 1;
    gameOver = false;
    pause = false;
}

static TetrisInput Tetris_GetInput() {

}

static void Tetris_Update(TetrisInput input) {

}

static void Draw_UI() {

}

static void Draw_Board() {

}

static void Draw_PauseScreen() {

}

static PieceType random_piece(void) {

}

static void spawn_piece() {

}

static bool check_collision(const Piece* p) {

}

static void lock_piece() {

}

static int clear_lines() {

}

static void update_level_and_speed() {

}