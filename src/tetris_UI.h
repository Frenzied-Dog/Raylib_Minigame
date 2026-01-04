#ifndef TETRIS_UI_H
#define TETRIS_UI_H

#include "tetris.h"
#include "raylib.h"

#define LETTER_ROWS 7
#define LETTER_COLS 5
#define MAX_PIECE_COUNT 18

void Draw_UI(const PieceType holdType, const bool holdLocked, const int score, const int level, const PieceType* bag, const int bagIndex, bool* pause, const bool gameOver);
void Draw_Board(const int board[22][10], const Piece current, const Piece shadow);
int Draw_PauseScreen(TetrisState* state);
void DrawPiecePreview(PieceType type, Rectangle box);
void UI_SetLayout();
int DrawMenu();
int DrawResultsScreen(int score, int totalLinesCleared, int level);

void getBoardGrid(int* ox, int* oy, int* cell, int* gridW, int* gridH);

static const int L_T[LETTER_ROWS][LETTER_COLS] = {
	{1,1,1,1,1},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
};
static const int L_E[LETTER_ROWS][LETTER_COLS] = {
	{1,1,1,1,1},
	{1,0,0,0,0},
	{1,0,0,0,0},
	{1,1,1,1,0},
	{1,0,0,0,0},
	{1,0,0,0,0},
	{1,1,1,1,1},
};
static const int L_R[LETTER_ROWS][LETTER_COLS] = {
	{1,1,1,1,0},
	{1,0,0,0,1},
	{1,0,0,0,1},
	{1,1,1,1,0},
	{1,0,1,0,0},
	{1,0,0,1,0},
	{1,0,0,0,1},
};
static const int L_I[LETTER_ROWS][LETTER_COLS] = {
	{1,1,1,1,1},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{0,0,1,0,0},
	{1,1,1,1,1},
};
static const int L_S[LETTER_ROWS][LETTER_COLS] = {
	{0,1,1,1,1},
	{1,0,0,0,0},
	{1,0,0,0,0},
	{0,1,1,1,0},
	{0,0,0,0,1},
	{0,0,0,0,1},
	{1,1,1,1,0},
};

typedef struct {
	Vector2 pos;      // top-left
	float speed;
	int type;         // 0..6
	int rot;          // 0..3
	int cell;
	Color color;
} FallingPiece;

#endif // TETRIS_UI_H