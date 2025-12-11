#ifndef TETRIS_H
#define TETRIS_H

#include "common.h"

#define TETRIS_BOARD_W 10
#define TETRIS_BOARD_H 20

void tetris(menuState* mainState);

typedef enum {
	PIECE_I = 0,
	PIECE_O,
	PIECE_T,
	PIECE_S,
	PIECE_Z,
	PIECE_J,
	PIECE_L,
	PIECE_NONE
} PieceType;

typedef struct {
	PieceType type;
	int rotation;   // 0~3
	int x;          // 在 board 上的 x (0 ~ TETRIS_BOARD_W-1)
	int y;          // 在 board 上的 y (可以 <0 表示還在畫面上方)
} Piece;

typedef struct {
	int move;           // -1: 左移一次, 1: 右移一次, 0: 不動
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

static void Tetris_Init();
static TetrisInput Tetris_GetInput();
static void Tetris_Update(TetrisInput input);
static void Draw_UI();
static void Draw_Board();
static void Draw_PauseScreen();
static PieceType random_piece(void);
static void spawn_piece();
static bool check_collision(const Piece* p);
static void lock_piece();
static int clear_lines();
static void update_level_and_speed();


#endif // TETRIS_H