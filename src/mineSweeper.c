#include "raylib.h"
#include "raygui.h"
#include "mineSweeper.h"

void minesweeper(menuState* state) {
	SetWindowSize(600, 400);
	while (!WindowShouldClose() && *state == STATE_MINESWEEPER) {
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Minesweeper Game Placeholder");
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
			*state = MAIN_MENU;
		EndDrawing();
	}
}	