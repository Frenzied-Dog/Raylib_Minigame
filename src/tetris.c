#include "raylib.h"
#include "raygui.h"
#include "tetris.h"

void tetris(menuState *mainState) {
    SetWindowSize(600, 400);
    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#OWO")) 
            *mainState = MAIN_MENU;
        EndDrawing();
    }
}