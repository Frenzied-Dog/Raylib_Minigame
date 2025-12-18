#ifndef TETRIS_H
#define TETRIS_H

#include "common.h"

#define TETRIS_BOARD_W 10
#define TETRIS_BOARD_H 22
#define LOCK_DELAY_FRAMES 30
#define LOCK_RESET_MAX 15
#define DAS_DELAY 8      // frames before auto-shift starts
#define ARR 2             // frames between auto-shifts
#define DCD 5             // frames between DAS moves during delayed auto-shift (DAS Cancel Delay)
#define SOFT_interval 2   // frames between soft drop moves

void tetris(menuState* mainState);

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
    PLAYING,
    GAMEOVER
} TetrisState;

#endif // TETRIS_H