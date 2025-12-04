#include "dodge.h"
#include "raylib.h"
#include "raygui.h"

void dodge(menuState* state) {
	SetWindowSize(600, 400);
	while (!WindowShouldClose() && *state == STATE_DODGE) {
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Dodge Game Placeholder");
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
			*state = MAIN_MENU;
		EndDrawing();
	}
}