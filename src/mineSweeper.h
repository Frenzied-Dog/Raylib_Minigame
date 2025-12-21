#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include "common.h"

void minesweeper(menuState* ); //main
void MSmenu(menuState* ); //menu
void MSgame(); //game logic
void MSgameDraw(int, int, int **, int **); //game drawing
void floodFill(int, int, int, int, int **,int **); //flood fill logic

#endif // MINESWEEPER_H

//GuiLabel((Rectangle){200, 180, 200, 40}, "Place mouse over input box to input");