#include "snake.h"
#include "raylib.h"
#include "raygui.h"

int SNAKE_WIDTH = 600;
int SNAKE_HEIGHT = 400;

void snake(menuState* mainState) {
	SetWindowSize(SNAKE_WIDTH, SNAKE_HEIGHT);
	while (!WindowShouldClose() && *mainState == STATE_SNAKE) {
		fixWindowDPI(SNAKE_WIDTH, SNAKE_HEIGHT);

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Snake Game Placeholder");
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
			*mainState = MAIN_MENU;
		EndDrawing();
	}
}