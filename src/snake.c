#include "snake.h"
#include "raylib.h"
#include "raygui.h"

static int SNAKE_WIDTH = 800;
static int SNAKE_HEIGHT = 600;

void snake(menuState* mainState) {
    SetWindowSize(SNAKE_WIDTH, SNAKE_HEIGHT);

    // [新增] 1. 載入音效資源
    // 注意：請確保你的 main.c 裡面有呼叫 InitAudioDevice(); 否則聲音不會響
    Sound fxButton = LoadSound("resources/button.mp3"); // 請準備按鈕音效
    Music bgm = LoadMusicStream("resources/game_bgm.mp3"); // 請準備背景音樂
    
    // 設定音樂循環播放
    bgm.looping = true; 
    float volume = 0.5f; // 音量 50%
    SetMusicVolume(bgm, volume);

    // --- 遊戲變數 ---
    GameScreen currentScreen = SCREEN_MENU; 
    
    // 蛇的資料
    Vector2 snake[MAX_SNAKE_LENGTH];
    int snakeLength = 3;
    Vector2 speed = {1, 0};
    Vector2 food = {15, 15};
    int framesCounter = 0;
    
    // [新增] 2. 定義移動速度變數 (原本是寫死 10)
    // 數字越大越慢，數字越小越快
    int moveDelay = 10; 
    
    // 計分
    int score = 0;
    int highScore = 0; 

    // --- 選單變數 (保持不變) ---
    int activeColorIndex = 0; 
    bool editMode = false; 
    const char *colorText = "Green;Orange;Blue;Purple;Dark Gray";
    Color colorOptions[] = { DARKGREEN, ORANGE, BLUE, PURPLE, DARKGRAY };

    while (!WindowShouldClose() && *mainState == STATE_SNAKE) {
        fixWindowDPI(SNAKE_WIDTH, SNAKE_HEIGHT);

        // [新增] 3. 更新音樂串流 (這行一定要放在 while 迴圈的最外層)
        // 只有在遊戲進行中才播放音樂，所以我們會根據狀態來決定是否 update
        if (currentScreen == SCREEN_GAMEPLAY) {
            UpdateMusicStream(bgm);
        }

        switch(currentScreen) {
            case SCREEN_MENU:
            {
                // [新增] 確保在選單時音樂是停止的
                StopMusicStream(bgm);
            } break;

            case SCREEN_GAMEPLAY:
            {
                // [新增] 確保進入遊戲時音樂開始播放
                if (!IsMusicStreamPlaying(bgm)) PlayMusicStream(bgm);

                // --- 蛇的操控邏輯 (保持不變) ---
                if (IsKeyPressed(KEY_RIGHT) && speed.x == 0) { speed.x = 1; speed.y = 0; }
                if (IsKeyPressed(KEY_LEFT)  && speed.x == 0) { speed.x = -1; speed.y = 0; }
                if (IsKeyPressed(KEY_UP)    && speed.y == 0) { speed.x = 0; speed.y = -1; }
                if (IsKeyPressed(KEY_DOWN)  && speed.y == 0) { speed.x = 0; speed.y = 1; }

                framesCounter++;
                
                // [修改] 4. 使用變數 moveDelay 來控制速度
                if (framesCounter >= moveDelay) 
                {
                    framesCounter = 0;

                    // 移動身體 (保持不變)
                    for (int i = snakeLength - 1; i > 0; i--) {
                        snake[i] = snake[i - 1];
                    }
                    snake[0].x += speed.x;
                    snake[0].y += speed.y;

                    // 1. 吃到食物
                    if (snake[0].x == food.x && snake[0].y == food.y) {
                        snakeLength++;
                        score += 100;
                        
                        // [新增] 5. 難度調整算法：分數越高，延遲越低 (速度越快)
                        // 原始速度 10，每得 200 分，延遲減少 1
                        // 最低延遲限制在 4 (不然會快到人類反應不過來)
                        moveDelay = 10 - (score / 200); 
                        if (moveDelay < 4) moveDelay = 4;

                        food.x = GetRandomValue(0, (SNAKE_WIDTH/SQUARE_SIZE) - 1);
                        food.y = GetRandomValue(0, (SNAKE_HEIGHT/SQUARE_SIZE) - 1);
                    }

                    // 2. 死亡檢測 (保持不變)
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
                        if (score > highScore) highScore = score;
                        currentScreen = SCREEN_GAMEOVER;
                        // [新增] 死亡時停止音樂
                        StopMusicStream(bgm);
                    }
                }
            } break;

            case SCREEN_GAMEOVER:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    // [新增] 按下 Enter 的音效
                    PlaySound(fxButton);
                    currentScreen = SCREEN_MENU;
                }
                if (IsKeyPressed(KEY_Q)) {
                    *mainState = MAIN_MENU;
                }
            } break;
        }

        // ====================================================================
        // [繪圖區] Draw
        // ====================================================================
        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch(currentScreen) 
            {
                case SCREEN_MENU: 
                {
                    DrawText("SNAKE GAME", 260, 100, 50, DARKGRAY);
                    DrawText("Select your snake color:", 290, 200, 20, GRAY);
                    
                    if (GuiButton((Rectangle) { 40, 30, 120, 30 }, "#191#Back to Menu")) {
                        PlaySound(fxButton); // [新增] 按鈕音效
                        *mainState = MAIN_MENU;
                    }

                    if (GuiComboBox((Rectangle){ 300, 230, 200, 40 }, colorText, &activeColorIndex)) {
                        PlaySound(fxButton); // [新增] 下拉選單音效
                        editMode = !editMode;
                    }

                    if (!editMode) 
                    {
                        if (GuiButton((Rectangle){ 300, 350, 200, 50 }, "START GAME")) {
                            PlaySound(fxButton); // [新增] 開始按鈕音效

                            // 重置遊戲數據
                            snakeLength = 3;
                            score = 0;
                            moveDelay = 10; // [新增] 重置速度回慢速
                            
                            for (int i = 0; i < snakeLength; i++) {
                                snake[i].x = 10 - i;
                                snake[i].y = 10;
                            }
                            speed.x = 1; speed.y = 0;
                            currentScreen = SCREEN_GAMEPLAY;
                        }
                    }
                    DrawText(TextFormat("High Score: %04i", highScore), 320, 500, 20, LIGHTGRAY);
                } break;

                case SCREEN_GAMEPLAY:
                {
                    // (繪圖部分保持不變)
                    for (int i = 0; i < SNAKE_WIDTH/SQUARE_SIZE + 1; i++)
                        DrawLineV((Vector2){SQUARE_SIZE*i, 0}, (Vector2){SQUARE_SIZE*i, SNAKE_HEIGHT}, Fade(LIGHTGRAY, 0.5f));
                    for (int i = 0; i < SNAKE_HEIGHT/SQUARE_SIZE + 1; i++)
                        DrawLineV((Vector2){0, SQUARE_SIZE*i}, (Vector2){SNAKE_WIDTH, SQUARE_SIZE*i}, Fade(LIGHTGRAY, 0.5f));

                    DrawRectangle(food.x * SQUARE_SIZE, food.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);

                    Color currentSnakeColor = colorOptions[activeColorIndex];
                    for (int i = 0; i < snakeLength; i++) {
                        DrawRectangle(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, currentSnakeColor);
                        DrawRectangleLines(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.3f));
                    }

                    DrawText(TextFormat("SCORE: %04i", score), 20, 20, 30, DARKGRAY);
                    // [選用] 也可以把目前速度顯示出來 debug 用
                    // DrawText(TextFormat("Speed Delay: %d", moveDelay), 20, 50, 20, LIGHTGRAY);
                } break;

                case SCREEN_GAMEOVER:
                {
                    DrawText("GAME OVER", 280, 200, 50, RED);
                    DrawText(TextFormat("Final Score: %i", score), 320, 280, 20, DARKGRAY);
                    DrawText("Press ENTER to return to Menu", 240, 350, 20, GRAY);
                } break;
            }

        EndDrawing();
    }

    // [新增] 6. 離開函式前，記得釋放音效資源！
    UnloadSound(fxButton);
    UnloadMusicStream(bgm);
    // 注意：CloseAudioDevice() 通常放在 main.c 的最後面，不要在這裡關閉，不然其他遊戲會沒聲音
}