#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raygui.h"
#include "common.h"

#include "menu.h"
#include "tetris.h"
#include "snake.h"
#include "mineSweeper.h"
#include "dodge.h"

int main() {
	printf("raygui controls test suite\n");	
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(400, 200, "NCKU Raylib MiniGames");
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(KEY_NULL);
	SetWindowIcon(LoadImage("Raylib_logo.png"));

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
			snake(&state);
			break;
		case STATE_MINESWEEPER:
			minesweeper(&state);
			break;
		case STATE_DODGE:
			dodge(&state);
			break;
		default:
			state = MAIN_MENU;
			break;
		}
	}

	CloseAudioDevice();
	CloseWindow();
	return 0;
}