#ifndef SNAKE_H
#define SNAKE_H

#include "common.h"
#define SQUARE_SIZE 20
#define MAX_SNAKE_LENGTH 256

void snake(menuState* mainState);

// 定義遊戲狀態：選單、遊戲中、遊戲結束
typedef enum { 
    SCREEN_MENU, 
    SCREEN_GAMEPLAY, 
    SCREEN_GAMEOVER 
} GameScreen;


#endif // SNAKE_H