#include "dodge.h"
#include "raylib.h"
#include "raygui.h"

int DODGE_WIDTH = 600;
int DODGE_HEIGHT = 400;

void dodge(menuState* mainState) {
	SetWindowSize(DODGE_WIDTH, DODGE_HEIGHT);
	while (!WindowShouldClose() && *mainState == STATE_DODGE) {
		fixWindowDPI(DODGE_WIDTH, DODGE_HEIGHT);
		
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Dodge Game Placeholder");
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
			*mainState = MAIN_MENU;
		EndDrawing();
	}
}