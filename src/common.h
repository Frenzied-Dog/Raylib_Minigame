#ifndef COMMON_H
#define COMMON_H

typedef enum {
	MAIN_MENU,
	STATE_TETRIS,
	STATE_SNAKE,
	STATE_MINESWEEPER,
	STATE_DODGE
} menuState;

void fixWindowDPI(int baseWidth, int baseHeight);

#endif // COMMON_H