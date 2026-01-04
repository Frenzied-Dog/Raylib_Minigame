#ifndef TETRIS_H
#define TETRIS_H

#include "common.h"

#define TETRIS_WINDOW_WIDTH 850
#define TETRIS_WINDOW_HEIGHT 650
#define TETRIS_BOARD_W 10
#define TETRIS_BOARD_H 22
#define LOCK_DELAY_FRAMES 30
#define LOCK_RESET_MAX 15
#define DAS_DELAY 8      // frames before auto-shift starts
#define ARR 2             // frames between auto-shifts
#define DCD 5             // frames between DAS moves during delayed auto-shift (DAS Cancel Delay)
#define SOFT_interval 2   // frames between soft drop moves

#define BGM_COUNT 3
#define EXPLODE_MAX_PARTICLES (TETRIS_BOARD_W*TETRIS_BOARD_H + 32)

void tetris(menuState* mainState);

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Color color;
    bool onGround;
    bool active;
} BlockParticle;

typedef enum {
    PIECE_NONE = -1,
    PIECE_I,
    PIECE_O,
    PIECE_T,
    PIECE_S,
    PIECE_Z,
    PIECE_J,
    PIECE_L
} PieceType;

typedef struct {
    PieceType type;
    int rotation;   // 0~3
    int x;          // 在 board 上的 x
    double y;          // 在 board 上的 y
    bool onGround;   // 是否已著地
} Piece;

typedef struct {
    int left;
    int right;
    bool softDrop;      // true: 一格一格快速下落
    bool hardDrop;      // true: 直接到底
    bool rotateCW;      // 順時針旋轉
    bool rotateCCW;     // 逆時針旋轉
    bool hold;          // 是否使用 hold（如果你不想做 hold，可以忽略）
} TetrisInput;

typedef enum {
    MENU,
    SINGLE,
    GAMEOVER_ANIM,
    RESULTS
} TetrisState;

static const double G[] = { 0.01667, 0.021017, 0.026977, 0.035256, 0.04693, 0.06361, 0.0879, 0.1236, 0.1775, 0.2598, 0.388, 0.59, 0.92, 1.46, 2.36 }; // 等級對應重力 (幀/格)
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
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
    },
    // PIECE_T
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 0 } },
        { { 1, 0 }, { 0, 0 }, { -1, 0 }, { 0, 1 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, 0 } },
    },
    // PIECE_S
     {
        { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, -1 }, { 0, 0 }, { 1, 0 }, { 1, 1 } },
        { { 1, 0 }, { 0, 0 }, { 0, 1 }, { -1, 1 } },
        { { 0, 1 }, { 0, 0 }, { -1, 0 }, { -1, -1 } },
    },
    // PIECE_Z
    {
        { { -1, -1 }, { 0, -1 }, { 0, 0 }, { 1, 0 } },
        { { 1, -1 }, { 1, 0 }, { 0, 0 }, { 0, 1 } },
        { { 1, 1 }, { 0, 1 }, { 0, 0 }, { -1, 0 } },
        { { -1, 1 }, { -1, 0 }, { 0, 0 }, { 0, -1 } },
    },
    // PIECE_J
    {
        { { -1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } },
        { { 1, -1 }, { 0, -1 }, { 0, 0 }, { 0, 1 } },
        { { 1, 1 }, { 1, 0 }, { 0, 0 }, { -1, 0 } },
        { { -1, 1 }, { 0, 1 }, { 0, 0 }, { 0, -1 } },
    },
    // PIECE_L
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
        { { 1, 0 }, { 0, 0 }, { -1, 0 }, { -1, 1 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, -1 } },
    },
};

// 中低彩度（比 raylib 預設柔和，但不會太灰）
static const Color pieceColors[7] = {
    (Color){ 90, 170, 210, 255}, // soft cyan/sky
    (Color){235, 200,  80, 255}, // soft gold
    (Color){175, 105, 200, 255}, // soft purple
    (Color){ 95, 195, 110, 255}, // soft green
    (Color){220,  95,  95, 255}, // soft red
    (Color){ 85, 115, 220, 255}, // soft blue
    (Color){235, 155,  75, 255}, // soft orange
};


#endif // TETRIS_H