#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raygui.h"
#include "const.h"

#include "menu.h"
#include "tetris.h"
// #include "snake.h"
// #include "mineSweeper.h"

int main()
{
	printf("raygui controls test suite\n");	
	InitWindow(400, 200, "NCKU Raylib MiniGames");
	SetTargetFPS(60);

	menuState state = MAIN_MENU;
	while (!WindowShouldClose()) {
		printf("Current State: %d\n", state);
		switch (state) {
		case MAIN_MENU:
			menu(&state);
			break;
		case STATE_TETRIS:
			tetris(&state);
			break;
		case STATE_SNAKE:
			// TODO:
			break;
		case STATE_MINESWEEPER:
			// TODO:
			break;
		case STATE_DASH:
			// TODO:
			break;
		default:
			state = MAIN_MENU;
			break;
		}
	}

	CloseWindow();
	return 0;
}