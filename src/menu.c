#include "raylib.h"
#include "raygui.h"
#include "menu.h"

int MENU_WIDTH = 450;
int MENU_HEIGHT = 300;

void menu(menuState *mainState) {
	SetWindowSize(MENU_WIDTH, MENU_HEIGHT);
	while (!WindowShouldClose() && *mainState == MAIN_MENU) {
		fixWindowDPI(MENU_WIDTH, MENU_HEIGHT);

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