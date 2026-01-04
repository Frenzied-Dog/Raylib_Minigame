#ifndef COMMON_H
#define COMMON_H
#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)

typedef enum {
	MAIN_MENU,
	STATE_TETRIS,
	STATE_SNAKE,
	STATE_MINESWEEPER,
	STATE_DODGE,
	EXIT	
} menuState;

void fixWindowDPI(int baseWidth, int baseHeight);
float Randf(float a, float b);

#endif // COMMON_H