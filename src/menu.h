#ifndef MENU_H
#define MENU_H

#include "common.h"

void menu(menuState* state);

typedef struct {
	Vector2 pos;
	Vector2 vel;
	float size;
	float alpha;
	Color col;
} DecoBlock;

#endif // MENU_H