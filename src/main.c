#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raygui.h"
#include "const.h"

#include "tetris.h"
// #include "snake.h"
// #include "mineSweeper.h"

int main()
{
	printf("raygui controls test suite\n");	
	InitWindow(400, 200, "NCKU Raylib MiniGames");
	SetTargetFPS(60);

	bool showMessageBox = false;
	menuState state = MAIN_MENU;
	while (!WindowShouldClose())
	{
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		switch (state) {
		case MAIN_MENU:
			SetWindowSize(400, 200);
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			if (GuiButton((Rectangle) { 24, 24, 120, 30 }, "#191#Show Message"))
				showMessageBox = true;

			if (showMessageBox) {
				int result = GuiMessageBox((Rectangle) { 85, 70, 250, 100 },
					"#191#Message Box", "Hi! This is a message!", "Nice;Cool");

				// printf("MessageBox result: %d\n", result);
				if (result >= 0) showMessageBox = false;
				if (result == 2) state = STATE_TETRIS;
			}
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


		EndDrawing();
	}

	CloseWindow();
	return 0;
}