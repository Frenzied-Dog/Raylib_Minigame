#include "raylib.h"
#include "raygui.h"
#include "tetris.h"

int TETRIS_WIDTH = 600;
int TETRIS_HEIGHT = 400;

void tetris(menuState *mainState) {
    SetWindowSize(TETRIS_WIDTH, TETRIS_HEIGHT);
    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        fixWindowDPI(TETRIS_WIDTH, TETRIS_HEIGHT);

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Tetris Game Placeholder");
        if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu"))
            *mainState = MAIN_MENU;
        EndDrawing();
    }
}