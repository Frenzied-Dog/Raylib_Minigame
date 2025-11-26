#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "raylib.h"
#include "raygui.h"

int main()
{
	printf("raygui controls test suite\n");	
	InitWindow(400, 200, "raygui - controls test suite");
	SetTargetFPS(60);

	bool showMessageBox = false;
	int state = 0;
	while (!WindowShouldClose())
	{
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		if (state == 0) {
			SetWindowSize(400, 200);
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			if (GuiButton((Rectangle) { 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

			if (showMessageBox) {
				int result = GuiMessageBox((Rectangle) { 85, 70, 250, 100 },
					"#191#Message Box", "Hi! This is a message!", "Nice;Cool");

				// printf("MessageBox result: %d\n", result);
				if (result >= 0) showMessageBox = false;
				if (result == 2) state = 1;
			}
		} else {
			SetWindowSize(600, 400);
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#OWO")) state = 0;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}