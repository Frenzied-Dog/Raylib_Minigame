#include "dodge.h"
#include "raylib.h"
#include "raygui.h"

#define MAX_OBSTACLES 24

static const int DODGE_WIDTH = 600;
static const int DODGE_HEIGHT = 400;

typedef struct Obstacle {
    Rectangle rect;
    float speed;
    bool active;
} Obstacle;

static void ResetDodge(Rectangle *player, Obstacle obstacles[], int *score,
	bool *gameRunning, bool *gameOver,
	float *spawnTimer, float *spawnInterval)
{
*player = (Rectangle){ DODGE_WIDTH/2.0f - 30, DODGE_HEIGHT - 45, 60, 16 };

for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;

*score = 0;
*gameRunning = true;
*gameOver = false;

*spawnTimer = 0.0f;
*spawnInterval = 0.70f;
}

void dodge(menuState* mainState) {
	SetWindowSize(DODGE_WIDTH, DODGE_HEIGHT);
	//遊戲狀態
	Rectangle player = { 0 };
    float playerSpeed = 320.0f; // px/sec
    Obstacle obstacles[MAX_OBSTACLES] = { 0 };

    int score = 0;
    bool gameRunning = true;
    bool gameOver = false;

    float spawnTimer = 0.0f;
    float spawnInterval = 0.70f;

	ResetDodge(&player, obstacles, &score, &gameRunning, &gameOver, &spawnTimer, &spawnInterval);

	while (!WindowShouldClose() && *mainState == STATE_DODGE) {
		fixWindowDPI(DODGE_WIDTH, DODGE_HEIGHT);
		float dt = GetFrameTime();
		if (gameRunning && !gameOver)
        {
            // 玩家移動（左右）
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  player.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.x += playerSpeed * dt;

            // 邊界限制
            if (player.x < 0) player.x = 0;
            if (player.x + player.width > DODGE_WIDTH) player.x = DODGE_WIDTH - player.width;

            // 生成障礙物
            spawnTimer += dt;
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0.0f;

                // 隨著分數提高略微加快生成（可自行改）
                if (score > 200 && spawnInterval > 0.45f) spawnInterval = 0.60f;
                if (score > 500 && spawnInterval > 0.35f) spawnInterval = 0.50f;

                for (int i = 0; i < MAX_OBSTACLES; i++)
                {
                    if (!obstacles[i].active)
                    {
                        float w = (float)(20 + GetRandomValue(0, 35)); // 20~55
                        float h = (float)(12 + GetRandomValue(0, 20)); // 12~32
                        float x = (float)GetRandomValue(0, (int)(DODGE_WIDTH - (int)w));
                        obstacles[i].rect = (Rectangle){ x, -h - 5, w, h };
                        obstacles[i].speed = 160.0f + (float)GetRandomValue(0, 180); // 160~340
                        obstacles[i].active = true;
                        break;
                    }
                }
            }

            // 更新障礙物 + 碰撞
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (!obstacles[i].active) continue;

                obstacles[i].rect.y += obstacles[i].speed * dt;

                // 掉出畫面：加分並回收
                if (obstacles[i].rect.y > DODGE_HEIGHT)
                {
                    obstacles[i].active = false;
                    score += 10;
                    continue;
                }

                // 撞到玩家：Game Over
                if (CheckCollisionRecs(player, obstacles[i].rect))
                {
                    gameOver = true;
                    gameRunning = false;
                }
            }
        }
		
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		
		GuiLabel((Rectangle){ 200, 180, 200, 40 }, "Dodge Game Placeholder");
		// Back to Menu
		if (GuiButton((Rectangle) { 80, 80, 120, 30 }, "#191#Back to Menu")) 
		{
			*mainState = MAIN_MENU;
		}
		// Restart / Start / Pause
        if (GuiButton((Rectangle){ 180, 35, 120, 30 }, "Restart"))
        {
            ResetDodge(&player, obstacles, &score, &gameRunning, &gameOver, &spawnTimer, &spawnInterval);
        }

        if (!gameOver)
        {
            if (GuiButton((Rectangle){ 320, 35, 90, 30 }, "Start")) gameRunning = true;
            if (GuiButton((Rectangle){ 420, 35, 90, 30 }, "Pause")) gameRunning = false;
        }

        // 分數顯示
        DrawText(TextFormat("Score: %d", score), 520, 15, 18, BLACK);

        // 遊戲區塊背景
        DrawRectangle(0, 75, DODGE_WIDTH, DODGE_HEIGHT - 75, RAYWHITE);

        // 玩家
        DrawRectangleRec(player, BLUE);

        // 障礙物
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (obstacles[i].active) DrawRectangleRec(obstacles[i].rect, RED);
        }

        // 狀態提示
        if (gameOver)
        {
            DrawText("GAME OVER", DODGE_WIDTH/2 - 90, DODGE_HEIGHT/2 - 25, 30, RED);
            DrawText("Click Restart", DODGE_WIDTH/2 - 70, DODGE_HEIGHT/2 + 10, 18, DARKGRAY);
        }
        else if (!gameRunning)
        {
            DrawText("Press Start or move with LEFT/RIGHT", 120, DODGE_HEIGHT - 28, 16, DARKGRAY);
        }
		EndDrawing();
	}
}
