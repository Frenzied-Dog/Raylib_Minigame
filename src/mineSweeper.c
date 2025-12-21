#include "raylib.h"
#include "raygui.h"
#include "mineSweeper.h"

bool menuOrGame = false;//false: menu, true: game
//bool gameShouldEnd = false;
int mode = -1;//0: beginner mode, 1: intermediate mode, 2: expert mode
int MS_WIDTH = 600;
int MS_HEIGHT = 400;
int BEGINNER_GAME_WIDTH = 9;
int BEGINNER_GAME_HEIGHT = 9;
int BEGINNER_MINES = 10;
int INTERMEDIATE_GAME_WIDTH = 16;
int INTERMEDIATE_GAME_HEIGHT = 16;
int INTERMEDIATE_MINES = 40;
int EXPERT_GAME_WIDTH = 30;
int EXPERT_GAME_HEIGHT = 16;
int EXPERT_MINES = 99;

void minesweeper(menuState* mainState){
	menuOrGame = false;
	//gameShouldEnd = false;
	mode = -1;
	while(!WindowShouldClose() && *mainState == STATE_MINESWEEPER){
		if(!menuOrGame){
			MSmenu(mainState);
		}else{
			MSgame();
		}
	}
}	

void MSmenu(menuState* mainState){
	SetWindowSize(MS_WIDTH, MS_HEIGHT);
	while(!WindowShouldClose() && *mainState == STATE_MINESWEEPER && !menuOrGame){
		fixWindowDPI(MS_WIDTH, MS_HEIGHT);
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		if(GuiButton((Rectangle){10, 10, 120, 30}, "#191#Back to Menu")){ 
			*mainState = MAIN_MENU;
		}
		if(GuiButton((Rectangle){210, 100, 180, 45}, "Beginner mode")){ 
			mode = 0;
			menuOrGame = true;
		}
		if(GuiButton((Rectangle){210, 200, 180, 45}, "Intermediate mode")){ 
			mode = 1;
			menuOrGame = true;
		}
		if(GuiButton((Rectangle){210, 300, 180, 45}, "Expert mode")){ 
			mode = 2;
			menuOrGame = true;
		}
		EndDrawing();
	}
}

void MSgame(){
	//set width, height, mine qunatity for different mode
	int gameWidth, gameHeight, mines;
	switch(mode){
		case 0:
			gameWidth = BEGINNER_GAME_WIDTH;
			gameHeight = BEGINNER_GAME_HEIGHT;
			mines = BEGINNER_MINES;
			break;
		case 1:
			gameWidth = INTERMEDIATE_GAME_WIDTH;
			gameHeight = INTERMEDIATE_GAME_HEIGHT;
			mines = INTERMEDIATE_MINES;
			break;
		case 2:
			gameWidth = EXPERT_GAME_WIDTH;
			gameHeight = EXPERT_GAME_HEIGHT;
			mines = EXPERT_MINES;
			break;
		default:
			gameWidth = BEGINNER_GAME_WIDTH;
			gameHeight = BEGINNER_GAME_HEIGHT;
			mines = BEGINNER_MINES;
			break;
	}

	SetWindowSize(gameWidth * 30, gameHeight * 30);

	//initializing data structure
	int map[gameHeight][gameWidth]; //stores mine info, -1: mine, -2: first step
	for(int i = 0; i < gameHeight; i ++){
		for(int j = 0; j < gameWidth; j ++){
			map[i][j] = 0;
		}
    }
	int step[gameHeight][gameWidth]; //stores which places have been stepped on or flagged, 0: untouched, 1: stepped, 2: flagged
	for(int i = 0; i < gameHeight; i ++){
		for(int j = 0; j < gameWidth; j ++){
			step[i][j] = 0;
		}
    }
	
	//initilize mine placement
	int mix;
	int miy;
	srand((unsigned int)time(0));
	for(int i = 0; i < mines;){
		mix = rand() % gameWidth;
		miy = rand() % gameHeight;
		if(map[miy][mix] != -1){
			map[miy][mix] = -1;
			i ++;
		}
	}

	//calculate numbers for all locations
	for(int i = 0; i < gameWidth; i ++){
		for(int j = 0; j < gameHeight; j ++){
			if(map[j][i] != -1){
				for(int k = i - 1; k <= i + 1; k ++){
					for(int l = j - 1; l <= j + 1; l ++){
						if(k == i && l == j){
							continue;
						}else if(k < 0 || l < 0 || k > gameWidth - 1 || l > gameHeight - 1){
							continue;
						}else if(map[l][k] == -1){
							map[j][i] ++;
						}
					}
				}
			}
		}
	}

	int *mapptr[gameHeight];
	for(int i = 0; i < gameHeight; i ++){
		mapptr[i] = map[i];
	}
	int *stepptr[gameHeight];
	for(int i = 0; i < gameHeight; i ++){
		stepptr[i] = step[i];
	}

	while(!WindowShouldClose() && /*!gameShouldEnd &&*/ menuOrGame){
		MSgameDraw(gameWidth, gameHeight, mapptr, stepptr);
		//
	}
}

void MSgameDraw(int gameWidth, int gameHeight, int **map, int **step){
	fixWindowDPI(gameWidth * 30, gameHeight * 30 + 40);
	BeginDrawing();
	ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
	if(GuiButton((Rectangle){10, 10, 100, 40}, "exit")){
		menuOrGame = false;
	}
	//
	EndDrawing();
}

void floodFill(int gameWidth, int gameHeight, int x, int y, int **map, int **step){
	//
}