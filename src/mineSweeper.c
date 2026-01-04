#include "raylib.h"
#include "raygui.h"
#include "mineSweeper.h"

static const int MS_WIDTH = 600;
static const int MS_HEIGHT = 400;
static const int TOP_MENU_HEIGHT = 40;
static const int SQUARE_SIDE_LENGTH = 30;
static const int BEGINNER_GAME_WIDTH = 9;
static const int BEGINNER_GAME_HEIGHT = 9;
static const int BEGINNER_MINES = 10;
static const int INTERMEDIATE_GAME_WIDTH = 16;
static const int INTERMEDIATE_GAME_HEIGHT = 16;
static const int INTERMEDIATE_MINES = 40;
static const int EXPERT_GAME_WIDTH = 30;
static const int EXPERT_GAME_HEIGHT = 16;
static const int EXPERT_MINES = 99;

void minesweeper(menuState* mainState){
	bool menuOrGame = false; //false: menu, true: game
	int mode = -1; //0: beginner mode, 1: intermediate mode, 2: expert mode
	while(!WindowShouldClose() && *mainState == STATE_MINESWEEPER){
		if(!menuOrGame){
			MSmenu(mainState, &mode, &menuOrGame);
		}else{
			MSgame(mode, &menuOrGame);
		}
	}
}	

void MSmenu(menuState *mainState, int *mode, bool *menuOrGame){
	SetWindowSize(MS_WIDTH, MS_HEIGHT);
	while(!WindowShouldClose() && *mainState == STATE_MINESWEEPER && !*menuOrGame){
		fixWindowDPI(MS_WIDTH, MS_HEIGHT);
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		if(GuiButton((Rectangle){10, 10, 120, 30}, "#191#Back to Menu")){ 
			*mainState = MAIN_MENU;
		}
		if(GuiButton((Rectangle){210, 100, 180, 45}, "Beginner mode")){ 
			*mode = 0;
			*menuOrGame = true;
		}
		if(GuiButton((Rectangle){210, 200, 180, 45}, "Intermediate mode")){ 
			*mode = 1;
			*menuOrGame = true;
		}
		if(GuiButton((Rectangle){210, 300, 180, 45}, "Expert mode")){ 
			*mode = 2;
			*menuOrGame = true;
		}
		EndDrawing();
	}
}

void MSgame(int mode, bool *menuOrGame){
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

	int screenWidth = gameWidth * SQUARE_SIDE_LENGTH;
	int screenHeight = gameHeight * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT;
	SetWindowSize(screenWidth, screenHeight);

	//initializing data structure
	int map[gameHeight][gameWidth]; //stores mine info, -1: mine
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

	//start game
	bool firststep = true;
	bool end = false;
	bool win = false;
	while(!WindowShouldClose() && *menuOrGame){
		MSgameDraw(gameWidth, gameHeight, mapptr, stepptr, menuOrGame);
		//flag
		if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && inRange(GetMouseX(), 0, screenWidth - 1) && inRange(GetMouseY(), TOP_MENU_HEIGHT, screenHeight - 1) && !end){
			int x, y;
			x = GetMouseX() / SQUARE_SIDE_LENGTH;
			y = (GetMouseY() - TOP_MENU_HEIGHT) / SQUARE_SIDE_LENGTH;
			if(step[y][x] != 2){
				step[y][x] = 2;
			}else{
				step[y][x] = 0;
			}
		}
		//step
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && inRange(GetMouseX(), 0, screenWidth - 1) && inRange(GetMouseY(), TOP_MENU_HEIGHT, screenHeight - 1) && !end){
			int x, y;
			x = GetMouseX() / SQUARE_SIDE_LENGTH;
			y = (GetMouseY() - TOP_MENU_HEIGHT) / SQUARE_SIDE_LENGTH;
			if(map[y][x] == -1){
				end = true;
				for(int i = 0; i < gameWidth; i ++){
					for(int j = 0; j < gameHeight; j ++){
						if(map[j][i] == -1){
							step[j][i] = 1;
						}
					}
				}
			}else if(map[y][x] > 0){
				step[y][x] = 1;
			}else{
				floodFill(gameWidth, gameHeight, x, y, mapptr, stepptr);
			}
			int notStepped = 0;
			for(int i = 0; i < gameWidth; i ++){
				for(int j = 0; j < gameHeight; j ++){
					if(step[i][j] != 1){
						notStepped ++;
					}
				}
			}
			if(notStepped == mines){
				win = true;
				end = true;
				for(int i = 0; i < gameWidth; i ++){
					for(int j = 0; j < gameHeight; j ++){
						step[i][j] = 1;
					}
				}
			}
		}
	}
}

void MSgameDraw(int gameWidth, int gameHeight, int **map, int **step, bool *menuOrGame){
	int screenWidth = gameWidth * SQUARE_SIDE_LENGTH;
	int screenHeight = gameHeight * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT;
	Color textColor[9] = {
		BLANK,                       //0
		(Color){0, 102, 204, 255},   //1
		(Color){0, 153, 0, 255},     //2
		(Color){204, 0, 0, 255},     //3
		(Color){102, 0, 153, 255},   //4
		(Color){153, 0, 51, 255},    //5
		(Color){0, 153, 153, 255},   //6
		BLACK,                       //7
		(Color){96, 96, 96, 255}     //8
	};
	int textWidth;
	int fontSize = 20;
	fixWindowDPI(screenWidth, screenHeight);
	BeginDrawing();
	ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
	if(GuiButton((Rectangle){10, 10, 50, 20}, "exit")){
		*menuOrGame = false;
	}
	//draw squares
	for(int i = 0; i < gameWidth; i ++){
		for(int j = 0; j < gameHeight; j ++){
			if(step[j][i] == 0){
				if(inRange(GetMouseX(), i * SQUARE_SIDE_LENGTH, (i + 1) * (SQUARE_SIDE_LENGTH) - 1) && inRange(GetMouseY(), j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT, (j + 1) * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT - 1)){
					DrawRectangle(i * SQUARE_SIDE_LENGTH, j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT, SQUARE_SIDE_LENGTH, SQUARE_SIDE_LENGTH, (Color){225, 223, 218, 255});
				}else{
					DrawRectangle(i * SQUARE_SIDE_LENGTH, j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT, SQUARE_SIDE_LENGTH, SQUARE_SIDE_LENGTH, LIGHTGRAY);
				}
			}else if(step[j][i] == 1){
				DrawRectangle(i * SQUARE_SIDE_LENGTH, j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT, SQUARE_SIDE_LENGTH, SQUARE_SIDE_LENGTH, WHITE);
				if(map[j][i] != -1){
					textWidth = MeasureText(TextFormat("%d", map[j][i]), fontSize);
					DrawText(TextFormat("%d", map[j][i]), i * SQUARE_SIDE_LENGTH + ((SQUARE_SIDE_LENGTH - textWidth) / 2), j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT + ((SQUARE_SIDE_LENGTH - fontSize) / 2), fontSize, textColor[map[j][i]]);
				}else{
					textWidth = MeasureText("X", fontSize);
					DrawText("X", i * SQUARE_SIDE_LENGTH + ((SQUARE_SIDE_LENGTH - textWidth) / 2), j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT + ((SQUARE_SIDE_LENGTH - fontSize) / 2), fontSize, RED);
				}
			}else if(step[j][i] == 2){
				DrawRectangle(i * SQUARE_SIDE_LENGTH, j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT, SQUARE_SIDE_LENGTH, SQUARE_SIDE_LENGTH, LIGHTGRAY);
				textWidth = MeasureText("F", fontSize);
				DrawText("F", i * SQUARE_SIDE_LENGTH + ((SQUARE_SIDE_LENGTH - textWidth) / 2), j * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT + ((SQUARE_SIDE_LENGTH - fontSize) / 2), fontSize, DARKGRAY);
			}
		}
	}
	//draw lines
	for(int i = 0; i <= gameWidth; i ++){
		DrawLineEx((Vector2){i * SQUARE_SIDE_LENGTH, TOP_MENU_HEIGHT}, (Vector2){i * SQUARE_SIDE_LENGTH, screenHeight}, 2.0f, GRAY);
	}
	for(int i = 0; i <= gameHeight; i ++){
		DrawLineEx((Vector2){0, i * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT}, (Vector2){screenWidth, i * SQUARE_SIDE_LENGTH + TOP_MENU_HEIGHT}, 2.0f, GRAY);
	}
	EndDrawing();
}

void floodFill(int gameWidth, int gameHeight, int x, int y, int **map, int **step){
	step[y][x] = 1;
    for(int i = y - 1; i <= y + 1; i ++){
        for(int j = x - 1; j <= x + 1; j ++){
            if(i == y && j == x){
                continue;
            }else if(i < 0 || j < 0 || i > gameHeight - 1 || j > gameWidth - 1){
                continue;
            }else if(step[i][j] == 0 && map[i][j] != 0){
                step[i][j] = 1;
            }else if(map[i][j] == 0 && step[i][j] == 0){
                floodFill(gameWidth, gameHeight, j, i, map, step);
            }
        }
    }
}

bool inRange(int a, int lowBound, int highBound){
	if(a >= lowBound && a <= highBound){
		return true;
	}else{
		return false;
	}
}