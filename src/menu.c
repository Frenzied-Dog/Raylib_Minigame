#include "raylib.h"
#include "raygui.h"
#include "menu.h"


void menu(menuState *mainState) {
	bool showMessageBox = false;
	SetWindowSize(400, 200);
	while (!WindowShouldClose() && *mainState == MAIN_MENU) {
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		if (GuiButton((Rectangle) { 24, 24, 120, 30 }, "#191#Show Message"))
			showMessageBox = true;

		if (showMessageBox) {
			int result = GuiMessageBox((Rectangle) { 85, 70, 250, 100 },
				"#191#Message Box", "Hi! This is a message!", "Nice;Cool");

			// printf("MessageBox result: %d\n", result);
			if (result >= 0) showMessageBox = false;
			if (result == 2) *mainState = STATE_TETRIS;
		}
		EndDrawing();
	}
}