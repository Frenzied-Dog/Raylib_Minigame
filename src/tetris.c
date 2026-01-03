#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <stdio.h>
#include <math.h>

#include "raylib.h"
#include "raygui.h"
#include "tetris.h"
#include "tetris_UI.h"

static TetrisState state = MENU;
static int board[TETRIS_BOARD_H][TETRIS_BOARD_W]; // 0=空，其它代表方塊種類
static Piece current = { PIECE_NONE, 0, 0, 0.0, false };
static Piece shadow = { PIECE_NONE, 0, 0, 0.0, false };
static PieceType holdType = PIECE_NONE;
static bool holdLocked = false; // 本次落下是否已使用 hold
static int lockDelay = LOCK_DELAY_FRAMES;
static int lockResetCount = LOCK_RESET_MAX;
static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int level = 1;
static int bagIndex = 0;
static PieceType bag[14]; // 7-bag 系統
static int totalLinesCleared = 0;
static Sound click_move_softDrop, click_spin_hold_hardDrop, lose, comboSound[12];

#define BGM_COUNT 3
static Music bgm[BGM_COUNT];
static int bgmIndex = 0;

// ---- Game Over Explode (particles) ----
#define EXPLODE_MAX_PARTICLES (TETRIS_BOARD_W*TETRIS_BOARD_H + 32)
static BlockParticle explodeParticles[EXPLODE_MAX_PARTICLES];
static int explodePhase = 0; // 0: not init, 1: freeze, 2: explode

static void BGM_update(const bool isPaused);
static void Tetris_Init();
static TetrisInput Tetris_GetInput();
static void Tetris_Update(TetrisInput input);
static void DAS(int counter, bool isRight, bool disableDAS);
static void random_piece(bool forSecondBag);
static bool check_collision(const Piece* p);
static Piece rotate_piece(Piece rp, bool clockwise);
static void reset_lock_delay();
static void lock_piece();
static int clear_lines();
static void update_score(int linesCleared);
static void spawn_piece();

// game over explode animation
static void GameOverExplode_UpdateDraw(void);

void tetris(menuState *mainState) {
    SetRandomSeed((unsigned int)time(NULL));
    GuiLoadStyleDefault();
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    UI_SetLayout();
    GuiSetIconScale(2);
    state = MENU;
    
    // Preload sounds
    float preVolume = GetMasterVolume();
    SetMasterVolume(0.3f);
    Music theme_bgm = LoadMusicStream("resources/Tetris/Tetris.ogg");
    SetMusicVolume(theme_bgm, 0.3f);
    PlayMusicStream(theme_bgm);
    for (int i = 0; i < BGM_COUNT; i++) {
        bgm[i] = LoadMusicStream(TextFormat("resources/Tetris/tetris_bgm%d.ogg", i + 1));
        bgm[i].looping = false;
        SetMusicVolume(bgm[i], 0.3f);
    }
    
    click_move_softDrop = LoadSound("resources/Tetris/click1.ogg"); SetSoundVolume(click_move_softDrop, 0.5f);
    click_spin_hold_hardDrop = LoadSound("resources/Tetris/click2.ogg"); SetSoundVolume(click_spin_hold_hardDrop, 0.3f);
    lose = LoadSound("resources/Tetris/lose.ogg"); SetSoundVolume(lose, 0.4f);

    for (int i = 0; i < 8; i++) {
        comboSound[i] = LoadSound(TextFormat("resources/Tetris/combo-%d.ogg", i + 1));
    }

    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        fixWindowDPI(TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        bool screenShotRequested = false;
        int ret = -1;
        switch(state) {
        case MENU:
            UpdateMusicStream(theme_bgm);
            ret = DrawMenu();
            switch(ret) {
            case 0: // Start Game
                Tetris_Init();
                state = SINGLE;
                StopMusicStream(theme_bgm);
                PlayMusicStream(bgm[bgmIndex]);
                break;
            case 3: // Back to Menu
                *mainState = MAIN_MENU;
                StopMusicStream(theme_bgm);
                break;
            default:
                break;
            }
            break;
        case SINGLE:
            Draw_Board(board, current, shadow);
            Draw_UI(holdType, holdLocked, score, level, bag, bagIndex, &pause, gameOver);

            if (!gameOver) {
                if (IsKeyPressed(KEY_ESCAPE)) pause = !pause;
                BGM_update(pause);

                if (!pause) {
                    Tetris_Update(Tetris_GetInput());
                } else {
                    int ret = Draw_PauseScreen(&state);
                    if (ret >= 0) pause = false;

                    if (ret == 1) { // Restart
                        Tetris_Init();
                        state = SINGLE;
                        SeekMusicStream(bgm[bgmIndex], 0.0);
                    } else if (ret == 2) { // Menu
                        StopMusicStream(bgm[bgmIndex]);
                        PlayMusicStream(theme_bgm);
                        state = MENU;
                    }
                }
            } else {
                if (IsMusicStreamPlaying(bgm[bgmIndex])) StopMusicStream(bgm[bgmIndex]);
                state = GAMEOVER_ANIM;
                explodePhase = 0; // reset animation on entry
            }
            break;
        case GAMEOVER_ANIM:
            GameOverExplode_UpdateDraw();
            break;
        case RESULTS: 
            Draw_Board(board, current, shadow);
            Draw_UI(holdType, holdLocked, score, level, bag, bagIndex, &pause, gameOver);
            int r = DrawResultsScreen(score, totalLinesCleared, level);
            if (r == 0) { // Retry
                state = SINGLE;
                Tetris_Init();
                SeekMusicStream(bgm[bgmIndex], 0.0);
                PlayMusicStream(bgm[bgmIndex]);
            } else if (r == 1) { // Back to menu
                state = MENU;
                SeekMusicStream(theme_bgm, 0.0);
                PlayMusicStream(theme_bgm);
            } else if (r == 2) { // Screenshot
                screenShotRequested = true;
            }
            break;
        }
        
        EndDrawing();
        if (screenShotRequested) {
            TakeScreenshot(TextFormat("Tetris_Screenshot.png", score, level));
            screenShotRequested = false;
        }
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
    GuiSetIconScale(1);
    SetMasterVolume(preVolume);

    UnloadMusicStream(theme_bgm);
    for(int i = 0; i < BGM_COUNT; i++) UnloadMusicStream(bgm[i]);
    UnloadSound(click_move_softDrop); UnloadSound(click_spin_hold_hardDrop); UnloadSound(lose);
    for (int i = 0; i < 8; i++) UnloadSound(comboSound[i]);
}

static void Tetris_Init() {
    SetRandomSeed((unsigned int)time(NULL));
    srand(time(NULL));
    bgmIndex = GetRandomValue(0, BGM_COUNT - 1);
    score = 0;
    level = 1;
    totalLinesCleared = 0;
    gameOver = false;
    pause = false;
    holdLocked = false;
    holdType = PIECE_NONE;
    // current = (Piece){ bag[bagIndex], 0, 4, 1.0, false };
    for (int y = 0; y < TETRIS_BOARD_H; ++y) {
        for (int x = 0; x < TETRIS_BOARD_W; ++x) {
            board[y][x] = 0;
        }
    }
    bagIndex = -1;
    random_piece(false);
    random_piece(true);
    spawn_piece();
}

static TetrisInput Tetris_GetInput() {
    static int left = 0, right = 0;
    TetrisInput input = {0};

    // for DAS or other can hold: IsKeyDown
    // else IsKeyPressed
    if (IsKeyDown(KEY_LEFT)) {
        left++;
    } else left = 0;
    
    if (IsKeyDown(KEY_RIGHT)) {
        right++;
    } else right = 0;
    
    // avoid overflow
    if (left > 200 && right > 200) {
        left-=60;
        right-=60;
    } else if (left > 200 && right < 120) {
        left-=60;
    } else if (right > 200 && left < 120) {
        right-=60;
    }

    input.left = left;
    input.right = right;

    if (IsKeyPressed(KEY_SPACE)) {
        input.hardDrop = true;
    } else if (IsKeyDown(KEY_DOWN)) {
        input.softDrop = true;
    }

    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_UP)) {
        input.rotateCW = true;
    } else if (IsKeyPressed(KEY_Z)) {
        input.rotateCCW = true;
    }

    if (IsKeyPressed(KEY_C)) {
        input.hold = true;
    }

    // printf("Input - L:%d R:%d SD:%d HD:%d RCW:%d RCCW:%d H:%d\n", input.left, input.right, input.softDrop, input.hardDrop, input.rotateCW, input.rotateCCW, input.hold);

    return input;
}

static void Tetris_Update(TetrisInput input) {
    static int frame = 0;
    static int rot_hd_frame = -1;

    frame = (frame + 1) % 60;

    // hold
    if (input.hold && !holdLocked) {
        PieceType temp = holdType;
        holdType = current.type;
        if (temp == PIECE_NONE) {
            spawn_piece();
        } else {
            current = (Piece){ temp, 0, 4, 1.0, false };
            lockDelay = LOCK_DELAY_FRAMES;
            lockResetCount = LOCK_RESET_MAX;

            shadow = current;
            while (true) {
                shadow.y += 1.0;
                if (check_collision(&shadow)) {
                    shadow.y -= 1.0;
                    break;
                }
            }
        }
        holdLocked = true;
        PlaySound(click_spin_hold_hardDrop);
        return;
    }

    // left/right movement with DAS
    bool disableDAS = false;
    int diff = rot_hd_frame >= 0 ? ((frame - rot_hd_frame + 60) % 60) : -1;
    if (diff >= 0 && diff < DCD) disableDAS = true; // disable DAS for DCD frames after rotation/hold
    else rot_hd_frame = -1;

    if (input.left > 0 && input.right > 0) {
        if (input.right < input.left) { // 最後按右
            DAS(input.right, true, disableDAS);
        } else if (input.right > input.left) { // 最後按左
            DAS(input.left, false, disableDAS);
        }
    } else if (input.left > 0) {
        DAS(input.left, false, disableDAS);
    } else if (input.right > 0) {
        DAS(input.right, true, disableDAS);
    }

    // rotation
    Piece rot = current;
    if (input.rotateCW) {
        rot = rotate_piece(rot, true);
    } else if (input.rotateCCW) {
        rot = rotate_piece(rot, false);
    }

    if (rot.rotation != current.rotation) {
        current = rot;
        reset_lock_delay();
        PlaySound(click_spin_hold_hardDrop);

        rot_hd_frame = frame;
        shadow = current;
        while (true) {
            shadow.y += 1.0;
            if (check_collision(&shadow)) {
                shadow.y -= 1.0;
                break;
            }
        }
    }

    // hard drop
    if (input.hardDrop) {
        while (true) {
            Piece dropped = current;
            dropped.y += 1.0;
            if (check_collision(&dropped)) {
                break;
            }
            current = dropped;
            score += 2;
        }
        lock_piece();
        update_score(clear_lines());

        Sound dropSound = LoadSoundAlias(click_spin_hold_hardDrop);
        SetSoundVolume(dropSound, 0.5f);
        SetSoundPitch(dropSound, 1.5f);
        PlaySound(dropSound);

        spawn_piece();
        rot_hd_frame = frame;

        // check game over
        if (check_collision(&current)) {
            gameOver = true;
        }
        return;
    }

    // soft drop
    if (input.softDrop && !current.onGround && G[level - 1] < (1.0 / SOFT_interval) && frame % SOFT_interval == 0) {
        Piece moved = current;
        moved.y = (int)moved.y + 1;
        if (!check_collision(&moved)) {
            Sound dropSound = LoadSoundAlias(click_move_softDrop);
            SetSoundVolume(dropSound, 0.2f);
            PlaySound(dropSound);
            current = moved;
            score += 1;
        }
    }

    // check onGround
    Piece down = current;
    down.y += 1.0;
    bool coll = check_collision(&down);
    if (coll && !current.onGround) {
        current.onGround = true;
    } else if (!coll && current.onGround) {
        current.onGround = false;
    }


    // apply gravity
    if (!current.onGround) {
        double remaining = G[level - 1];
        while (remaining > 0.0) {
            double step = (remaining >= 1.0) ? 1.0 : remaining;

            Piece moved = current;
            moved.y += step;

            if (check_collision(&moved)) {
                // 撞到：停在目前格，標記著地。
                current.onGround = true;
                break;
            }

            current = moved;
            remaining -= step;
        }
    } else {
        current.y = (int)current.y;
        lockDelay--;
    }

    if (lockDelay <= 0 && current.onGround) {
        lock_piece();
        update_score(clear_lines());
        spawn_piece();

        // check game over
        if (check_collision(&current)) {
            gameOver = true;
        }
    }
}

static void DAS(int counter, bool isRight, bool disableDAS) {
    if (counter == 1 || (counter > DAS_DELAY && (counter - DAS_DELAY) % ARR == 0 && !disableDAS)) {
        Piece moved = current;
        moved.x += (isRight ? 1 : -1);

        if (!check_collision(&moved)) {
            current = moved;
            reset_lock_delay();
            PlaySound(LoadSoundAlias(click_move_softDrop));

            shadow = current;
            while (true) {
                shadow.y += 1.0;
                if (check_collision(&shadow)) {
                    shadow.y -= 1.0;
                    break;
                }
            }
        }
    }
}

static void reset_lock_delay() {
    if (current.onGround && lockResetCount > 0) {
        lockDelay = LOCK_DELAY_FRAMES; // reset lock delay
        lockResetCount--;
    }
    if (lockResetCount == 0) {
        lockDelay = 0; // force lock
    }
}

static Piece rotate_piece(Piece rp, bool clockwise) {
    if (rp.type == PIECE_O) return rp; // O 不需要位移測試
    int oriRot = rp.rotation;
    if (clockwise) rp.rotation = (rp.rotation + 1) % 4;
    else rp.rotation = (rp.rotation + 3) % 4;
    int newRot = rp.rotation;

    if (!check_collision(&rp)) return rp;
    
    Vector2 offsets[4] = { {0,0}, {0,0}, {0,0}, {0,0} }; // 最多嘗試 4 種位移
    if (rp.type == PIECE_I) {
        if ((oriRot == 0 && clockwise) || (oriRot == 3 && !clockwise)) { // 0->R | L->2
            offsets[0] = (Vector2){ -2, 0 };
            offsets[1] = (Vector2){ 1, 0 };
            offsets[2] = (Vector2){ -2, 1 };
            offsets[3] = (Vector2){ 1, -2 };
        } else if ((oriRot == 1 && clockwise) || (oriRot == 0 && !clockwise)) { // R->2 | 0->L
            offsets[0] = (Vector2){ -1, 0 };
            offsets[1] = (Vector2){ 2, 0 };
            offsets[2] = (Vector2){ -1, -2 };
            offsets[3] = (Vector2){ 2, 1 };
        } else if ((oriRot == 2 && clockwise) || (oriRot == 1 && !clockwise)) { // 2->L | R->0
            offsets[0] = (Vector2){ 2, 0 };
            offsets[1] = (Vector2){ -1, 0 };
            offsets[2] = (Vector2){ 2, -1 };
            offsets[3] = (Vector2){ -1, 2 };
        } else if ((oriRot == 3 && clockwise) || (oriRot == 2 && !clockwise)) { // L->0 | 2->R
            offsets[0] = (Vector2){ 1, 0 };
            offsets[1] = (Vector2){ -2, 0 };
            offsets[2] = (Vector2){ 1, 2 };
            offsets[3] = (Vector2){ -2, -1 };
        }
    } else { // 其他方塊
        if (oriRot == 1) { // R -> ?
            offsets[0] = (Vector2){ 1, 0 };
            offsets[1] = (Vector2){ 1, 1 };
            offsets[2] = (Vector2){ 0, -2 };
            offsets[3] = (Vector2){ 1, -2 };
        } else if (oriRot == 3) { // L -> ?
            offsets[0] = (Vector2){ -1, 0 };
            offsets[1] = (Vector2){ -1, 1 };
            offsets[2] = (Vector2){ 0, -2 };
            offsets[3] = (Vector2){ -1, -2 };
        } else if (newRot == 1) { // ? -> R
            offsets[0] = (Vector2){ 1, 0 };
            offsets[1] = (Vector2){ 1, -1 };
            offsets[2] = (Vector2){ 0, 2 };
            offsets[3] = (Vector2){ 1, 2 };
        } else if (newRot == 3) { // ? -> L
            offsets[0] = (Vector2){ -1, 0 };
            offsets[1] = (Vector2){ -1, -1 };
            offsets[2] = (Vector2){ 0, 2 };
            offsets[3] = (Vector2){ -1, 2 };
        }
    }

    for (int i=0; i<4; ++i) {
        Piece test = rp;
        test.x += (int)offsets[i].x;
        test.y += (int)offsets[i].y;
        if (!check_collision(&test)) {
            return test;
        }
    }
    return current; // 無法旋轉，回傳原本的
}

static void spawn_piece() {
    bagIndex = (bagIndex + 1) % 14;
    current = (Piece){ bag[bagIndex], 0, 4, 1.0, false };
    holdLocked = false;
    lockDelay = LOCK_DELAY_FRAMES;
    lockResetCount = LOCK_RESET_MAX;

    shadow = current;
    while (true) {
        shadow.y += 1.0;
        if (check_collision(&shadow)) {
            shadow.y -= 1.0;
            break;
        }
    }
}

static void random_piece(bool forSecondBag) {
    int index = forSecondBag ? 7 : 0;

    int *seq = LoadRandomSequence(7, 0, 6);
    for (int i = 0; i < 7; ++i) 
        bag[index + i] = (PieceType) seq[i];

    UnloadRandomSequence(seq);
}

static bool check_collision(const Piece* p) {
    for (int i = 0; i < 4; ++i) {
        int bx = p->x + (int)SHAPES[p->type][p->rotation][i].x;
        int by = (int)p->y + (int)SHAPES[p->type][p->rotation][i].y;

        // 檢查邊界
        if (bx < 0 || bx >= TETRIS_BOARD_W) return true;
        if (by < 0 || by >= TETRIS_BOARD_H) return true;

        // 檢查已鎖定的方塊
        if (board[by][bx] != 0) return true;
    }
    return false;
}

static void lock_piece() {
    for (int i = 0; i < 4; ++i) {
        int bx = current.x + (int)SHAPES[current.type][current.rotation][i].x;
        int by = (int)current.y + (int)SHAPES[current.type][current.rotation][i].y;

        if (bx < 0 || bx >= TETRIS_BOARD_W) continue;
        if (by < 0 || by >= TETRIS_BOARD_H) continue;

        board[by][bx] = (int)(current.type) + 1; // 以 1..7 存
    }
}

static int clear_lines() {
    int linesCleared = 0;
    for (int y = TETRIS_BOARD_H-1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < TETRIS_BOARD_W; x++) {
            if (board[y][x] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            linesCleared++;
            totalLinesCleared++;
            // 清除該行
            for (int ty = y; ty > 0; ty--) {
                for (int x = 0; x < TETRIS_BOARD_W; x++) {
                    board[ty][x] = board[ty - 1][x];
                }
            }
            // 清空最上面一行
            for (int x = 0; x < TETRIS_BOARD_W; x++) {
                board[0][x] = 0;
            }
            // 回到同一行繼續檢查
            y++;
        }
    }

    level = min(totalLinesCleared / 10 + 1, 15);
    return linesCleared;
}

static void update_score(int linesCleared) {
    static int combo = -1;
    if (linesCleared > 0) {
        combo++;
        PlaySound(LoadSoundAlias(comboSound[min(combo, 8)]));
        Sound comboSnd = LoadSoundAlias(comboSound[min(combo, 8)]);
        SetSoundVolume(comboSnd, 0.3f * (linesCleared-1));
        PlaySound(comboSnd);
    } else {
        combo = -1;
    }


    switch (linesCleared) {
    case 1:
        score += 100 * level + 50 * combo * level;
        break;
    case 2:
        score += 300 * level + 50 * combo * level;
        break;
    case 3:
        score += 500 * level + 50 * combo * level;
        break;
    case 4:
        score += 800 * level + 50 * combo * level;
        break;
    default:
        break;
    }
}

// ============================ Game Over Explode: Missing Implementations ============================
static float Randf(float a, float b) {
    // raylib GetRandomValue is inclusive, we map to float range
    int r = GetRandomValue(0, 10000);
    float t = (float)r / 10000.0f;
    return a + (b - a) * t;
}

static void Explode_SpawnPiece(int* explodeCount) {
    int ox, oy, cell, gridW, gridH;
    getBoardGrid(&ox, &oy, &cell, &gridW, &gridH);

    const float size = (float)cell;   // ✅ 每顆粒子就是一格大小
    const float cx = (float)ox + (float)gridW * 0.5f;
    const float cy = (float)oy + (float)gridH * 0.5f;

    // 1) locked board blocks: 一格 = 一顆粒子
    for (int y = 0; y < TETRIS_BOARD_H; y++) {
        for (int x = 0; x < TETRIS_BOARD_W; x++) {
            int v = board[y][x];
            if (v <= 0) continue;
            if (*explodeCount >= EXPLODE_MAX_PARTICLES) continue;

            Color col = pieceColors[v-1];

            float px = (float)ox + (float)x * (float)cell;
            float py = (float)oy + (float)y * (float)cell;

            // outward impulse
            float bx = px + size * 0.5f;
            float by = py + size * 0.5f;
            float dx = bx - cx;
            float dy = by - cy;

            Vector2 vel = (Vector2){
                Randf(-280.0f, 280.0f) + dx * 0.45f,
                Randf(-650.0f, -260.0f) + dy * 0.15f
            };

            BlockParticle* p = &explodeParticles[(*explodeCount)++];
            p->pos = (Vector2){ px, py };
            p->vel = vel;
            p->color = col;
            p->onGround = false;
            p->active = true;
        }
    }

    if (*explodeCount >= EXPLODE_MAX_PARTICLES) return;

    // 2) current piece blocks: 只拆成它的 4 格（若在板內）
    if (current.type >= PIECE_I && current.type <= PIECE_L) {
        Color col = pieceColors[(int)current.type];

        for (int i = 0; i < 4; i++) {
            int bx = current.x + (int)SHAPES[current.type][current.rotation][i].x;
            int by = (int)current.y + (int)SHAPES[current.type][current.rotation][i].y;

            if (bx < 0 || bx >= TETRIS_BOARD_W) continue;
            if (by < 0 || by >= TETRIS_BOARD_H) continue;
            // if (board[by][bx] != 0) continue; // 避免跟鎖定方塊重複

            float px = (float)ox + (float)bx * (float)cell;
            float py = (float)oy + (float)by * (float)cell;

            float cx2 = px + size * 0.5f;
            float cy2 = py + size * 0.5f;
            float dx = cx2 - cx;
            float dy = cy2 - cy;

            Vector2 vel = (Vector2){
                Randf(-280.0f, 280.0f) + dx * 0.45f,
                Randf(-650.0f, -260.0f) + dy * 0.15f
            };

            BlockParticle* p = &explodeParticles[(*explodeCount)++];
            p->pos = (Vector2){ px, py };
            p->vel = vel;
            p->color = col;
            p->onGround = false;
            p->active = true;
        }
    }
}

static void GameOverExplode_UpdateDraw(void) {
    // Tunable parameters
    const float FREEZE_SEC = 1.0f;  // 爆炸前停頓
    const float GRAVITY = 1500.0f;   // 重力
    const float END_SEC = 4.5f;      // 最長動畫時間（保險）
    const float BOUNCE = 0.35f;      // 彈地係數
    const float FRICTION = 0.85f;    // 水平摩擦

    static int explodeCount = 0;
    static float explodeTimer = 0.0f;

    float dt = GetFrameTime();
    explodeTimer += dt;

    // overlay
    Draw_UI(holdType, holdLocked, score, level, bag, bagIndex, &pause, gameOver);
    DrawRectangle(0, 0, TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT, Fade(BLACK, 0.15f));
    Draw_Board(board, current, shadow);

    const char* title = "GAME OVER";
    int fs = 48;
    int tw = MeasureText(title, fs);
    DrawText(title, TETRIS_WINDOW_WIDTH / 2 - tw / 2, 170, fs, (Color) { 220, 70, 70, 255 });

    switch (explodePhase) {
    case 0:
        explodeTimer = 0;     // start timer
        for (int i = 0; i < EXPLODE_MAX_PARTICLES; i++) {
            explodeParticles[i].active = false;
        }
        explodeCount = 0;

        pause = false;
        explodePhase = 1;      // freeze
        break;
    case 1:
        // freeze phase
        if (explodeTimer >= FREEZE_SEC/2 && !IsSoundPlaying(lose))
            PlaySound(lose);

        if (explodeTimer >= FREEZE_SEC) {
            // spawn particles from current visuals
            Explode_SpawnPiece(&explodeCount);

            // clear board to avoid double-drawing with particles
            for (int y = 0; y < TETRIS_BOARD_H; y++)
                for (int x = 0; x < TETRIS_BOARD_W; x++)
                    board[y][x] = 0;

            current.type = PIECE_NONE;
            shadow.type = PIECE_NONE;

            explodePhase = 2;
            explodeTimer = 0.0f;
        }
        break;
    case 2:
        // explode phase
        // compute board pixel bounds for floor/walls
        int cell;
        getBoardGrid(NULL, NULL, &cell, NULL, NULL);

        float bottom = (float)TETRIS_WINDOW_HEIGHT;

        int alive = 0;

        for (int i = 0; i < explodeCount; i++) {
            BlockParticle* p = &explodeParticles[i];
            if (!p->active) continue;


            if (!p->onGround) {
                alive++;

                // integrate
                p->vel.y += GRAVITY * dt;
                p->pos.x += p->vel.x * dt;
                p->pos.y += p->vel.y * dt;

                // floor bounce
                if (p->pos.y + cell > bottom) {
                    p->pos.y = bottom - cell;
                    p->vel.y *= -BOUNCE;
                    p->vel.x *= FRICTION;

                    if (fabsf(p->vel.y) < 60.0f) p->vel.y = 0.0f;
                    if (fabsf(p->vel.x) < 10.0f) p->vel.x = 0.0f;
                }
            }

            if (p->vel.y == 0.0f && p->pos.y + cell >= bottom) {
                p->vel.x = 0.0f;
                p->pos.y = bottom - cell;
                p->onGround = true;
            }

            // draw
            Rectangle rec = (Rectangle){ p->pos.x, p->pos.y, cell, cell };
            DrawRectangleRec(rec, p->color);
            DrawRectangleLinesEx(rec, 1, BLACK);
        }

        // finish condition
        static bool ended = false;
        if (alive == 0 && !ended) {
            ended = true;
            explodeTimer = max(explodeTimer, END_SEC - 0.4f);
        }

        if (explodeTimer >= END_SEC) {
            ended = false;
            explodePhase = 0;
            explodeCount = 0;
            explodeTimer = 0.0f;
            state = RESULTS;
        }

        break;
    default:
        break;
    }
}

void BGM_update(const bool isPaused) {
    static float prevPlayed = 0.0f;
    Music* m = &bgm[bgmIndex];

    if (isPaused) {
        PauseMusicStream(*m);
        return;
    }

    ResumeMusicStream(*m);
    UpdateMusicStream(*m);

    float played = GetMusicTimePlayed(*m);

    // 播到後面突然變小（常見：跳回 0）=> 視為播完
    if (prevPlayed > 0.0f && played + 0.02f < prevPlayed) {
        bgmIndex = (bgmIndex + 1) % BGM_COUNT;
        SeekMusicStream(bgm[bgmIndex], 0.0f);
        PlayMusicStream(bgm[bgmIndex]);
        prevPlayed = 0.0f;
        return;
    }

    prevPlayed = played;
}