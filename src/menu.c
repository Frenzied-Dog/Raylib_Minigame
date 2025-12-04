#include "raylib.h"
#include "raygui.h"
#include "menu.h"


void menu(menuState *mainState) {
	SetWindowSize(450, 300);
	while (!WindowShouldClose() && *mainState == MAIN_MENU) {
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		if (GuiButton((Rectangle) { 24, 24, 120, 30 }, "#157#Tetris")) 
			*mainState = STATE_TETRIS;
		if (GuiButton((Rectangle) { 152, 24, 120, 30 }, "#157#Snake")) 
			*mainState = STATE_SNAKE;
		if (GuiButton((Rectangle) { 280, 24, 120, 30 }, "#157#Minesweeper")) 
			*mainState = STATE_MINESWEEPER;
		if (GuiButton((Rectangle) { 152, 64, 120, 30 }, "#157#Dodge")) 
			*mainState = STATE_DODGE;

		
		EndDrawing();
	}
}