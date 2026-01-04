#include "common.h"
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include <math.h>
 
#define MAX_OBSTACLES 24
#define MAX_PARTICLES 140

static const int DODGE_WIDTH  = 600;
static const int DODGE_HEIGHT = 400;

typedef struct Obstacle {
    Rectangle rect;
    float speed;
    bool active;
} Obstacle;

typedef struct Particle {
    Vector2 pos;
    Vector2 vel;
    float life;     // 0~1
    float size;
    Color color;
    bool active;
} Particle;

// 小型粒子
static void SpawnBurst(Particle p[], int count, Vector2 at, Color baseColor)
{
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++)
    {
        if (!p[i].active)
        {
            float vx = (float)GetRandomValue(-180, 180);
            float vy = (float)GetRandomValue(-260, -80);

            p[i].pos = at;
            p[i].vel = (Vector2){ vx, vy };
            p[i].life = 1.0f;
            p[i].size = (float)GetRandomValue(2, 5);
            p[i].color = baseColor;
            p[i].active = true;
            count--;
        }
    }
}

static void UpdateParticles(Particle p[], float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!p[i].active) continue;

        p[i].vel.y += 620.0f * dt;      // 重力
        p[i].pos.x += p[i].vel.x * dt;
        p[i].pos.y += p[i].vel.y * dt;

        p[i].life -= 1.7f * dt;
        if (p[i].life <= 0.0f) p[i].active = false;
    }
}

static void DrawParticles(Particle p[], Vector2 offset)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!p[i].active) continue;

        Color c = p[i].color;
        c.a = (unsigned char)(Clamp(p[i].life, 0.0f, 1.0f) * 255);

        DrawCircleV((Vector2){ p[i].pos.x + offset.x, p[i].pos.y + offset.y }, p[i].size, c);
    }
}

// 遊戲
static void ResetDodge(Rectangle *player,
                       Obstacle obstacles[],
                       Particle particles[],
                       int *score,
                       bool *gameRunning,
                       bool *gameOver,
                       float *spawnTimer,
                       float *spawnInterval,
                       float *shakeTime,
                       float *shakeStrength,
                       float *t)
{
    *player = (Rectangle){ DODGE_WIDTH/2.0f - 30, DODGE_HEIGHT - 50, 60, 16 };

    for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;

    *score = 0;
    *gameRunning = true;
    *gameOver = false;

    *spawnTimer = 0.0f;
    *spawnInterval = 0.70f;

    *shakeTime = 0.0f;
    *shakeStrength = 0.0f;

    *t = 0.0f;
}

// 支援
static void DrawBackground(float t)
{
    // 霓虹漸層背景
    DrawRectangleGradientV(0, 0, DODGE_WIDTH, DODGE_HEIGHT,
        (Color){ 18, 20, 35, 255 },
        (Color){ 10, 12, 22, 255 });

    // 動畫
    for (int y = 0; y < DODGE_HEIGHT; y += 6)
    {
        int a = 10 + (int)(10 * (sinf(t * 2.3f + y * 0.06f)));
        DrawRectangle(0, y, DODGE_WIDTH, 1, (Color){ 255, 255, 255, (unsigned char)a });
    }

    // 移動的星星
    for (int i = 0; i < 60; i++)
    {
        float x = fmodf(i * 97.0f + t * 40.0f, (float)DODGE_WIDTH);
        float y = fmodf(i * 53.0f + t * 22.0f, (float)DODGE_HEIGHT);
        DrawPixel((int)x, (int)y, (Color){ 200, 210, 255, 35 });
    }
}

static void DrawField(float t)
{
    Rectangle field = { 0, 75, DODGE_WIDTH, DODGE_HEIGHT - 75 };

    // 基底
    DrawRectangleRec(field, (Color){ 245, 245, 250, 255 });
    DrawRectangleLinesEx(field, 2, (Color){ 40, 40, 60, 60 });

    // 移動道路
    int laneStep = 80;
    int shift = (int)(t * 140) % laneStep;
    for (int x = -laneStep; x < DODGE_WIDTH + laneStep; x += laneStep)
    {
        DrawRectangle(x + shift, 75, 5, DODGE_HEIGHT - 75, (Color){ 70, 85, 140, 28 });
    }

    // 底部陰影
    DrawRectangleGradientV(0, DODGE_HEIGHT - 30, DODGE_WIDTH, 30,
        (Color){ 0, 0, 0, 0 },
        (Color){ 0, 0, 0, 60 });
}

// 主要遊戲迴圈
void dodge(menuState* mainState)
{
    SetWindowSize(DODGE_WIDTH, DODGE_HEIGHT);

    Rectangle player = { 0 };
    float playerSpeed = 340.0f; // px/sec

    Obstacle obstacles[MAX_OBSTACLES] = { 0 };
    Particle particles[MAX_PARTICLES] = { 0 };

    int score = 0;
    bool gameRunning = true;
    bool gameOver = false;

    float spawnTimer = 0.0f;
    float spawnInterval = 0.70f;

    // Screen shake
    float shakeTime = 0.0f;
    float shakeStrength = 0.0f;

    // 動態參數
    float t = 0.0f;

    ResetDodge(&player, obstacles, particles, &score, &gameRunning, &gameOver,
              &spawnTimer, &spawnInterval, &shakeTime, &shakeStrength, &t);

    while (!WindowShouldClose() && *mainState == STATE_DODGE)
    {
        fixWindowDPI(DODGE_WIDTH, DODGE_HEIGHT);

        float dt = GetFrameTime();
        t += dt;

        UpdateParticles(particles, dt);
        if (shakeTime > 0.0f) shakeTime -= dt;

        
        if (gameRunning && !gameOver)
        {
            // Player movement
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  player.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.x += playerSpeed * dt;

            // Clamp
            if (player.x < 0) player.x = 0;
            if (player.x + player.width > DODGE_WIDTH) player.x = DODGE_WIDTH - player.width;

            // Spawn obstacles
            spawnTimer += dt;
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0.0f;

                // Difficulty ramp
                if (score > 200 && spawnInterval > 0.60f) spawnInterval = 0.60f;
                if (score > 500 && spawnInterval > 0.50f) spawnInterval = 0.50f;
                if (score > 900 && spawnInterval > 0.45f) spawnInterval = 0.45f;

                for (int i = 0; i < MAX_OBSTACLES; i++)
                {
                    if (!obstacles[i].active)
                    {
                        float w = (float)(22 + GetRandomValue(0, 38));  // 22~60
                        float h = (float)(14 + GetRandomValue(0, 22));  // 14~36
                        float x = (float)GetRandomValue(0, (int)(DODGE_WIDTH - (int)w));

                        obstacles[i].rect = (Rectangle){ x, -h - 5, w, h };
                        obstacles[i].speed = 170.0f + (float)GetRandomValue(0, 210); // 170~380
                        obstacles[i].active = true;
                        break;
                    }
                }
            }

            // 障礙物+碰撞
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (!obstacles[i].active) continue;

                obstacles[i].rect.y += obstacles[i].speed * dt;

                // 顯示分數
                if (obstacles[i].rect.y > DODGE_HEIGHT)
                {
                    obstacles[i].active = false;
                    score += 10;

                    SpawnBurst(particles, 6,
                        (Vector2){ obstacles[i].rect.x + obstacles[i].rect.width/2.0f, (float)DODGE_HEIGHT - 3 },
                        (Color){ 255, 220, 120, 220 });

                    continue;
                }

                // 遊戲結束
                if (CheckCollisionRecs(player, obstacles[i].rect))
                {
                    gameOver = true;
                    gameRunning = false;

                    shakeTime = 0.35f;
                    shakeStrength = 10.0f;

                    SpawnBurst(particles, 28,
                        (Vector2){ player.x + player.width/2.0f, player.y + player.height/2.0f },
                        (Color){ 255, 120, 120, 230 });
                }
            }

            // 快速移動粒子效果
            if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && (GetRandomValue(0, 2) == 0))
            {
                SpawnBurst(particles, 1,
                    (Vector2){ player.x + player.width/2.0f, player.y + player.height },
                    (Color){ 140, 200, 255, 160 });
            }
        }

        // 晃動
        Vector2 cam = { 0 };
        if (shakeTime > 0.0f)
        {
            cam.x = (float)GetRandomValue(-(int)shakeStrength, (int)shakeStrength);
            cam.y = (float)GetRandomValue(-(int)shakeStrength, (int)shakeStrength);
        }

        // 繪圖
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawBackground(t);

        // UI效果
        DrawRectangle(0, 0, DODGE_WIDTH, 75, (Color){ 255, 255, 255, 12 });
        DrawLine(0, 74, DODGE_WIDTH, 74, (Color){ 255, 255, 255, 30 });

        GuiLabel((Rectangle){ 16, 10, 200, 20 }, "Dodge Game");

        // 按鍵
        if (GuiButton((Rectangle){ 16, 36, 140, 30 }, "#191#Back to Menu"))
            *mainState = MAIN_MENU;

        if (GuiButton((Rectangle){ 168, 36, 110, 30 }, "Restart"))
            ResetDodge(&player, obstacles, particles, &score, &gameRunning, &gameOver,
                      &spawnTimer, &spawnInterval, &shakeTime, &shakeStrength, &t);

        if (!gameOver)
        {
            if (GuiButton((Rectangle){ 292, 36, 90, 30 }, "Start")) gameRunning = true;
        }

        // HUD panel
        GuiPanel((Rectangle){ DODGE_WIDTH - 190, 30, 180, 70 }, "HUD"); // Moved higher by adjusting y from 50 to 30
        DrawText(TextFormat("Score: %d", score), DODGE_WIDTH - 175, 50, 20, (Color){ 50, 50, 50, 255 }); // Adjusted y position from 70 to 50
        
        // Field
        DrawField(t);

        // Player (with shadow + cam)
        DrawRectangle((int)(player.x + cam.x) + 2, (int)(player.y + cam.y) + 2,
                      (int)player.width, (int)player.height, (Color){ 0, 0, 0, 70 });
        DrawRectangleRec((Rectangle){ player.x + cam.x, player.y + cam.y, player.width, player.height },
                         (Color){ 80, 170, 255, 255 });
        DrawRectangleLinesEx((Rectangle){ player.x + cam.x, player.y + cam.y, player.width, player.height },
                            1, (Color){ 255, 255, 255, 120 });

        // Obstacles
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (!obstacles[i].active) continue;

            Rectangle r = obstacles[i].rect;
            r.x += cam.x;
            r.y += cam.y;

            DrawRectangle((int)r.x + 2, (int)r.y + 2, (int)r.width, (int)r.height, (Color){ 0, 0, 0, 80 });
            DrawRectangleRec(r, (Color){ 255, 90, 90, 255 });
            DrawRectangleLinesEx(r, 1, (Color){ 255, 255, 255, 130 });
        }

        // Particles
        DrawParticles(particles, cam);

        // Hint / Status text
        if (!gameOver && !gameRunning)
        {
            DrawText("Move: LEFT/RIGHT or A/D", 165, DODGE_HEIGHT - 26, 16, (Color){ 40, 40, 70, 130 });
        }

        // Game Over overlay
        if (gameOver)
        {
            DrawRectangle(0, 75, DODGE_WIDTH, DODGE_HEIGHT - 75, (Color){ 0, 0, 0, 150 });
            DrawText("GAME OVER", DODGE_WIDTH/2 - 98, DODGE_HEIGHT/2 - 34, 34, (Color){ 255, 110, 110, 255 });
            DrawText("Click Restart", DODGE_WIDTH/2 - 74, DODGE_HEIGHT/2 + 8, 18, (Color){ 240, 240, 240, 255 });
        }

        EndDrawing();
    }
}
