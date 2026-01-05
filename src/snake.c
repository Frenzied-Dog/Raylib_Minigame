#include "snake.h"
#include "raylib.h"
#include "raygui.h"

static int SNAKE_WIDTH = 800;
static int SNAKE_HEIGHT = 600;

// [畫背景] 淺咖啡底 + 稀疏的隨機小草 (保持不變)
void DrawSnakeBackground() {
    Color coffeeBg = (Color){ 235, 228, 205, 255 }; 
    Color gridLineCol = (Color){ 200, 190, 170, 150 };
    Color grassLight = (Color){ 130, 200, 100, 200 };
    Color grassDark =  (Color){ 100, 170, 80, 220 };

    ClearBackground(coffeeBg); 

    for (int i = 0; i <= SNAKE_WIDTH/SQUARE_SIZE; i++) {
        DrawLine(i*SQUARE_SIZE, 0, i*SQUARE_SIZE, SNAKE_HEIGHT, gridLineCol);
    }
    for (int j = 0; j <= SNAKE_HEIGHT/SQUARE_SIZE; j++) {
        DrawLine(0, j*SQUARE_SIZE, SNAKE_WIDTH, j*SQUARE_SIZE, gridLineCol);
    }

    for (int i = 0; i < SNAKE_WIDTH/SQUARE_SIZE; i++)
    {
        for (int j = 0; j < SNAKE_HEIGHT/SQUARE_SIZE; j++)
        {
            unsigned int seed = (i * 331 + j * 57 + i*j);
            if (seed % 37 == 0) {
                int rootX = i * SQUARE_SIZE;
                int rootY = j * SQUARE_SIZE + SQUARE_SIZE;
                int offsetX = (seed % 10) + 5; 

                DrawLineEx((Vector2){rootX + offsetX, rootY}, (Vector2){rootX + offsetX + 1, rootY - 12}, 2.0f, grassDark);
                DrawLineEx((Vector2){rootX + offsetX - 2, rootY}, (Vector2){rootX + offsetX - 5, rootY - 9}, 2.0f, grassLight);
                DrawLineEx((Vector2){rootX + offsetX + 2, rootY}, (Vector2){rootX + offsetX + 6, rootY - 10}, 2.0f, grassLight);
            }
        }
    }
}

void snake(menuState* mainState) {
    SetWindowSize(SNAKE_WIDTH, SNAKE_HEIGHT);

    // --- 1. 載入音效資源 ---
    Music bgm = LoadMusicStream("resources/game_bgm.mp3"); 
    Sound fxEat = LoadSound("resources/coin.mp3");
    
    // [新增] 載入按鈕與結束音效
    Sound fxButton = LoadSound("resources/button.mp3"); 
    Sound fxGameOver = LoadSound("resources/gameover.mp3");

    bgm.looping = true; 
    float volume = 0.4f; 
    SetMusicVolume(bgm, volume);

    // --- 遊戲變數 ---
    GameScreen currentScreen = SCREEN_MENU; 
    
    Vector2 snake[MAX_SNAKE_LENGTH];
    int snakeLength = 3;
    Vector2 speed = {1, 0};
    Vector2 food = {15, 15};
    int framesCounter = 0;
    
    int moveDelay = 10; 
    int currentLevel = 1;

    int score = 0;
    int highScore = 0; 

    bool dying = false;       
    float deathAlpha = 0.0f;  

    // --- 選單變數 ---
    int activeColorIndex = 0; 
    bool editMode = false; 
    
    // 顏色定義 (莫蘭迪色系)
    Color colOlive = (Color){ 110, 130, 70, 255 };
    Color colTerra = (Color){ 200, 100, 80, 255 };
    Color colSlate = (Color){ 90, 110, 140, 255 };
    Color colMauve = (Color){ 140, 90, 120, 255 };
    Color colCoffee = (Color){ 80, 60, 50, 255 };

    const char *colorText = "Olive Green;Terracotta;Slate Blue;Mauve;Espresso";
    Color colorOptions[] = { colOlive, colTerra, colSlate, colMauve, colCoffee };

    while (!WindowShouldClose() && *mainState == STATE_SNAKE) {
        fixWindowDPI(SNAKE_WIDTH, SNAKE_HEIGHT);

        if (currentScreen == SCREEN_GAMEPLAY) {
            UpdateMusicStream(bgm);
        }

        switch(currentScreen) {
            case SCREEN_MENU:
            {
                StopMusicStream(bgm);
            } break;

            case SCREEN_GAMEPLAY:
            {
                if (!dying && !IsMusicStreamPlaying(bgm)) PlayMusicStream(bgm);

                if (!dying) {
                    if (IsKeyPressed(KEY_RIGHT) && speed.x == 0) { speed.x = 1; speed.y = 0; }
                    if (IsKeyPressed(KEY_LEFT)  && speed.x == 0) { speed.x = -1; speed.y = 0; }
                    if (IsKeyPressed(KEY_UP)    && speed.y == 0) { speed.x = 0; speed.y = -1; }
                    if (IsKeyPressed(KEY_DOWN)  && speed.y == 0) { speed.x = 0; speed.y = 1; }

                    framesCounter++;
                    
                    if (framesCounter >= moveDelay) 
                    {
                        framesCounter = 0;

                        for (int i = snakeLength - 1; i > 0; i--) {
                            snake[i] = snake[i - 1];
                        }
                        snake[0].x += speed.x;
                        snake[0].y += speed.y;

                        if (snake[0].x == food.x && snake[0].y == food.y) {
                            PlaySound(fxEat); // 吃到東西
                            snakeLength++;
                            score += 100;
                            
                            int difficultyStep = score / 200; 
                            moveDelay = 10 - difficultyStep; 
                            if (moveDelay < 4) moveDelay = 4;
                            currentLevel = 1 + difficultyStep;

                            food.x = GetRandomValue(0, (SNAKE_WIDTH/SQUARE_SIZE) - 1);
                            food.y = GetRandomValue(0, (SNAKE_HEIGHT/SQUARE_SIZE) - 1);
                        }

                        bool collision = false;
                        if (snake[0].x < 0 || snake[0].x >= SNAKE_WIDTH/SQUARE_SIZE ||
                            snake[0].y < 0 || snake[0].y >= SNAKE_HEIGHT/SQUARE_SIZE) {
                            collision = true;
                        }
                        for (int i = 1; i < snakeLength; i++) {
                            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                                collision = true;
                            }
                        }

                        if (collision) {
                            if (!dying) {
                                dying = true;
                                StopMusicStream(bgm); 
                                
                                // [新增] 撞到時播放 Game Over 音效
                                PlaySound(fxGameOver); 
                            }
                        }
                    }
                } 
                else 
                {
                    deathAlpha += 0.02f; 
                    if (deathAlpha >= 1.0f) {
                        if (score > highScore) highScore = score;
                        currentScreen = SCREEN_GAMEOVER;
                        dying = false;
                        deathAlpha = 0.0f;
                    }
                }
            } break;

            case SCREEN_GAMEOVER:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(fxButton); // [新增] 按下 Enter 回選單的音效
                    currentScreen = SCREEN_MENU;
                }
                if (IsKeyPressed(KEY_Q)) {
                    PlaySound(fxButton); // [新增] 按下 Q 離開的音效
                    *mainState = MAIN_MENU;
                }
            } break;
        }

        BeginDrawing();
            DrawSnakeBackground();

            switch(currentScreen) 
            {
                case SCREEN_MENU: 
                {
                    DrawRectangle(200, 50, 400, 500, Fade(WHITE, 0.8f));
                    DrawRectangleLines(200, 50, 400, 500, (Color){180,170,160,255}); 

                    DrawText("SNAKE GAME", 230, 100, 50, (Color){80,70,60,255}); 
                    DrawText("Select your style:", 310, 200, 20, GRAY);
                    
                    if (GuiButton((Rectangle) { 40, 30, 120, 30 }, "#191#Back to Menu")) {
                        PlaySound(fxButton); // [新增] 按鈕音效
                        *mainState = MAIN_MENU;
                    }

                    if (GuiComboBox((Rectangle){ 300, 230, 200, 40 }, colorText, &activeColorIndex)) {
                        PlaySound(fxButton); // [新增] 下拉選單音效
                        editMode = !editMode;
                    }

                    Color previewColor = colorOptions[activeColorIndex];
                    int menuCenterX = 200 + 400 / 2; 
                    int previewY = 420;              
                    int previewSnakeLen = 5;         
                    int snakeTotalWidth = previewSnakeLen * SQUARE_SIZE; 
                    int startX = menuCenterX - (snakeTotalWidth / 2);

                    const char* previewText = "Preview Style:";
                    int textWidth = MeasureText(previewText, 20); 
                    DrawText(previewText, menuCenterX - textWidth / 2, 390, 20, GRAY);
                    
                    for(int k = 0; k < previewSnakeLen; k++) {
                        int currentX = startX + k * SQUARE_SIZE;
                        bool isHead = (k == previewSnakeLen - 1);
                        DrawRectangle(currentX, previewY, SQUARE_SIZE, SQUARE_SIZE, previewColor);
                        if (isHead) {
                            DrawRectangleLines(currentX, previewY, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.5f));
                            DrawRectangle(currentX + 12, previewY + 5, 4, 4, Fade(BLACK, 0.4f));
                        } else {
                            DrawRectangleLines(currentX, previewY, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.2f));
                        }
                    }

                    if (!editMode) 
                    {
                        if (GuiButton((Rectangle){ 300, 300, 200, 50 }, "START GAME")) {
                            PlaySound(fxButton); // [新增] 開始按鈕音效

                            snakeLength = 3;
                            score = 0;
                            moveDelay = 10;
                            currentLevel = 1; 
                            dying = false;     
                            deathAlpha = 0.0f; 
                            
                            for (int i = 0; i < snakeLength; i++) {
                                snake[i].x = 10 - i;
                                snake[i].y = 10;
                            }
                            speed.x = 1; speed.y = 0;
                            currentScreen = SCREEN_GAMEPLAY;
                        }
                    }
                    DrawText(TextFormat("High Score: %04i", highScore), 320, 500, 20, GRAY);
                } break;

                case SCREEN_GAMEPLAY:
                {
                    DrawRectangle(food.x * SQUARE_SIZE, food.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, colTerra); 
                    DrawRectangleLines(food.x * SQUARE_SIZE, food.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.3f));

                    Color currentSnakeColor = colorOptions[activeColorIndex];
                    for (int i = 0; i < snakeLength; i++) {
                        DrawRectangle(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, currentSnakeColor);
                        DrawRectangleLines(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.3f));
                    }

                    DrawRectangle(15, 15, 200, 70, Fade(WHITE, 0.6f));
                    DrawRectangleLines(15, 15, 200, 70, Fade(GRAY, 0.5f));
                    DrawText(TextFormat("SCORE: %04i", score), 25, 25, 30, (Color){60,60,60,255});
                    DrawText(TextFormat("LEVEL: %d", currentLevel), 25, 60, 20, (Color){100,100,100,255});

                    if (dying) {
                        DrawRectangle(0, 0, SNAKE_WIDTH, SNAKE_HEIGHT, Fade(colTerra, deathAlpha)); 
                        if (deathAlpha > 0.3f) {
                            DrawText("CRASHED!", SNAKE_WIDTH/2 - 100, SNAKE_HEIGHT/2 - 20, 40, WHITE);
                        }
                    }

                } break;

                case SCREEN_GAMEOVER:
                {
                    DrawRectangle(0, 0, SNAKE_WIDTH, SNAKE_HEIGHT, Fade(colCoffee, 0.5f)); 
                    DrawText("GAME OVER", 250, 180, 50, WHITE);
                    DrawText(TextFormat("Final Score: %i", score), 320, 260, 20, Fade(WHITE, 0.9f));
                    DrawText(TextFormat("Reached Level: %d", currentLevel), 310, 290, 20, Fade(WHITE, 0.9f));
                    DrawText("Press ENTER to return to Menu", 240, 360, 20, Fade(WHITE, 0.7f));
                } break;
            }

        EndDrawing();
    }

    // --- 釋放資源 ---
    UnloadMusicStream(bgm);
    UnloadSound(fxEat);
    
    // [新增] 記得釋放這兩個新音效
    UnloadSound(fxButton);
    UnloadSound(fxGameOver);
}