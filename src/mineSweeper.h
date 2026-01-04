#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include "common.h"
#include <stdlib.h>
#include <time.h>

void minesweeper(menuState *); //main
void MSmenu(menuState *, int *, bool *); //menu
void MSgame(int, bool *); //game logic
void MSgameDraw(int, int, int **, int **, bool *); //game drawing
void floodFill(int, int, int, int, int **, int **); //flood fill logic
bool inRange(int, int, int);

#endif // MINESWEEPER_H

//Ignore that below
//GuiLabel((Rectangle){200, 180, 200, 40}, "Place mouse over input box to input");