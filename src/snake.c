#include "snake.h"
#include "raylib.h"
#include "raygui.h"

static int SNAKE_WIDTH = 800;
static int SNAKE_HEIGHT = 600;

void snake(menuState* mainState) {
    SetWindowSize(SNAKE_WIDTH, SNAKE_HEIGHT);

        // --- 遊戲變數 ---
    GameScreen currentScreen = SCREEN_MENU; // 預設從選單開始
    
    // 蛇的資料
    Vector2 snake[MAX_SNAKE_LENGTH];
    int snakeLength = 3;
    Vector2 speed = {1, 0};
    Vector2 food = {15, 15};
    int framesCounter = 0;
    
    // 計分
    int score = 0;
    int highScore = 0; // 最高分紀錄

    // --- 選單相關變數 ---
    // Raygui 的 ComboBox 需要一個變數來存目前選到第幾項
    int activeColorIndex = 0; 
    bool editMode = false; // ComboBox 是否被點開的狀態
    
    // 定義可選的蛇顏色 (對應 ComboBox 的選項)
    const char *colorText = "Green;Orange;Blue;Purple;Dark Gray";
    Color colorOptions[] = { DARKGREEN, ORANGE, BLUE, PURPLE, DARKGRAY };


    while (!WindowShouldClose() && *mainState == STATE_SNAKE) {
        fixWindowDPI(SNAKE_WIDTH, SNAKE_HEIGHT);

        switch(currentScreen) {
            case SCREEN_MENU:
            {
                // 在選單畫面時，邏輯主要由 Raygui 的控制項處理 (在 Draw 區塊)
                // 這裡可以放一些簡單的背景動畫邏輯
            } break;

            case SCREEN_GAMEPLAY:
            {
                // --- 蛇的操控邏輯 (同之前) ---
                if (IsKeyPressed(KEY_RIGHT) && speed.x == 0) { speed.x = 1; speed.y = 0; }
                if (IsKeyPressed(KEY_LEFT)  && speed.x == 0) { speed.x = -1; speed.y = 0; }
                if (IsKeyPressed(KEY_UP)    && speed.y == 0) { speed.x = 0; speed.y = -1; }
                if (IsKeyPressed(KEY_DOWN)  && speed.y == 0) { speed.x = 0; speed.y = 1; }

                framesCounter++;
                if (framesCounter >= 10) 
                {
                    framesCounter = 0;

                    // 移動身體
                    for (int i = snakeLength - 1; i > 0; i--) {
                        snake[i] = snake[i - 1];
                    }
                    // 移動頭
                    snake[0].x += speed.x;
                    snake[0].y += speed.y;

                    // 1. 吃到食物
                    if (snake[0].x == food.x && snake[0].y == food.y) {
                        snakeLength++;
                        score += 100; // 加分！
                        food.x = GetRandomValue(0, (SNAKE_WIDTH/SQUARE_SIZE) - 1);
                        food.y = GetRandomValue(0, (SNAKE_HEIGHT/SQUARE_SIZE) - 1);
                    }

                    // 2. 死亡檢測 (撞牆 或 撞自己)
                    bool collision = false;
                    // 撞牆
                    if (snake[0].x < 0 || snake[0].x >= SNAKE_WIDTH/SQUARE_SIZE ||
                        snake[0].y < 0 || snake[0].y >= SNAKE_HEIGHT/SQUARE_SIZE) {
                        collision = true;
                    }
                    // 撞自己
                    for (int i = 1; i < snakeLength; i++) {
                        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                            collision = true;
                        }
                    }

                    if (collision) {
                        if (score > highScore) highScore = score; // 更新最高分
                        currentScreen = SCREEN_GAMEOVER; // 切換到遊戲結束畫面
                    }
                }
            } break;

            case SCREEN_GAMEOVER:
            {
                // 按下 Enter 回到選單
                if (IsKeyPressed(KEY_ENTER)) {
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
                // ------------------ 選單畫面 ------------------
                case SCREEN_MENU: 
                {
                    DrawText("SNAKE GAME", 260, 100, 50, DARKGRAY);
                    DrawText("Select your snake color:", 290, 200, 20, GRAY);

                    // 1. 顏色選擇下拉選單 (Raygui)
                    // 注意：如果 ComboBox 打開 (editMode為真)，其他控制項通常要鎖住，但在這裡我們簡單處理
                    if (GuiComboBox((Rectangle){ 300, 230, 200, 40 }, colorText, &activeColorIndex)) {
                        editMode = !editMode;
                    }

                    // 2. 開始遊戲按鈕
                    // 只有在下拉選單沒有打開時，才允許點擊按鈕
                    if (!editMode) 
                    {
                        if (GuiButton((Rectangle){ 300, 350, 200, 50 }, "START GAME")) {
                            // 重置遊戲數據
                            snakeLength = 3;
                            score = 0;
                            for (int i = 0; i < snakeLength; i++) {
                                snake[i].x = 10 - i;
                                snake[i].y = 10;
                            }
                            speed.x = 1; speed.y = 0;
                            
                            // 切換狀態
                            currentScreen = SCREEN_GAMEPLAY;
                        }
                    }
                    
                    // 顯示目前最高分
                    DrawText(TextFormat("High Score: %04i", highScore), 320, 500, 20, LIGHTGRAY);
                } break;

                // ------------------ 遊戲畫面 ------------------
                case SCREEN_GAMEPLAY:
                {
                    // 畫網格
                    for (int i = 0; i < SNAKE_WIDTH/SQUARE_SIZE + 1; i++)
                        DrawLineV((Vector2){SQUARE_SIZE*i, 0}, (Vector2){SQUARE_SIZE*i, SNAKE_HEIGHT}, Fade(LIGHTGRAY, 0.5f));
                    for (int i = 0; i < SNAKE_HEIGHT/SQUARE_SIZE + 1; i++)
                        DrawLineV((Vector2){0, SQUARE_SIZE*i}, (Vector2){SNAKE_WIDTH, SQUARE_SIZE*i}, Fade(LIGHTGRAY, 0.5f));

                    // 畫食物
                    DrawRectangle(food.x * SQUARE_SIZE, food.y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);

                    // 畫蛇 (使用選單選定的顏色)
                    Color currentSnakeColor = colorOptions[activeColorIndex];
                    for (int i = 0; i < snakeLength; i++) {
                        DrawRectangle(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, currentSnakeColor);
                        // 畫個黑框讓蛇身比較明顯
                        DrawRectangleLines(snake[i].x * SQUARE_SIZE, snake[i].y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, Fade(BLACK, 0.3f));
                    }

                    // 畫分數 UI
                    DrawText(TextFormat("SCORE: %04i", score), 20, 20, 30, DARKGRAY);
                } break;

                // ------------------ 結束畫面 ------------------
                case SCREEN_GAMEOVER:
                {
                    DrawText("GAME OVER", 280, 200, 50, RED);
                    DrawText(TextFormat("Final Score: %i", score), 320, 280, 20, DARKGRAY);
                    DrawText("Press ENTER to return to Menu", 240, 350, 20, GRAY);
                } break;
            }

        EndDrawing();
    }
}