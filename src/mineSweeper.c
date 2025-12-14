#include "raylib.h"
#include "raygui.h"
#include "mineSweeper.h"

int MS_WIDTH = 600;
int MS_HEIGHT = 400;

void minesweeper(menuState* mainState) {
	SetWindowSize(MS_WIDTH, MS_HEIGHT);
	while (!WindowShouldClose() && *mainState == STATE_MINESWEEPER) {
		fixWindowDPI(MS_WIDTH, MS_HEIGHT);

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Minesweeper Game Placeholder");
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
			*mainState = MAIN_MENU;
		EndDrawing();
	}
}	