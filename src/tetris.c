#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <stdio.h>

#include "raylib.h"
#include "raygui.h"
#include "tetris.h"
#include "tetris_UI.h"

static TetrisState state = MENU;
static int aniState = 0; // TODO: animation state

static int board[TETRIS_BOARD_H][TETRIS_BOARD_W]; // 0=空，其它代表方塊種類
static Piece current = {PIECE_NONE, 0, 0, 0.0, false};
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


void tetris(menuState *mainState) {
    SetRandomSeed((unsigned int)time(NULL));
    GuiLoadStyleDefault();
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    UI_SetLayout();

    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        fixWindowDPI(TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        int ret = -1;
        switch(state) {
        case MENU:
            ret = DrawMenu();
            switch(ret) {
            case 0: // Start Game
                Tetris_Init();
                state = SINGLE;
                break;
            case 3: // Back to Menu
                *mainState = MAIN_MENU;
                break;
            default:
                break;
            }
            // if (GuiButton((Rectangle) { 80, 90, 120, 30 }, "#191#Start Game")) {
            //     Tetris_Init();
            //     state = SINGLE;
            // }
            
            // if (GuiButton((Rectangle) { 80, 50, 140, 30 }, "#191#Back to Menu")) {
            //     *mainState = MAIN_MENU;
            // }
            break;
        case SINGLE:
            Draw_Board(board, current, shadow);
            Draw_UI(holdType, holdLocked, score, level, bag, bagIndex, &pause);

            if (!gameOver) {
                if (!pause) Tetris_Update(Tetris_GetInput());
                else {
                    int ret = Draw_PauseScreen(&pause, &state);
                    if (ret == 1) { // Restart
                        Tetris_Init();
                        state = SINGLE;
                    } else if (ret == 2) { // Menu
                        state = MENU;
                    }
                }
            } else {
                // TODO: Animation
                state = GAMEOVER;
            }
            break;
        case GAMEOVER:
            Draw_Board(board, current, shadow);
            Draw_UI(holdType, holdLocked, score, level, bag, bagIndex, &pause);
            DrawText("Game Over", TETRIS_WINDOW_WIDTH / 2 - 100, TETRIS_WINDOW_HEIGHT / 2 - 20, 40, RED);
            // TODO: Game Over Screen
            break;
        }
        
        EndDrawing();
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
}

static void Tetris_Init() {
    srand(time(NULL));
    score = 0;
    level = 1;
    totalLinesCleared = 0;
    gameOver = false;
    pause = false;
    holdLocked = false;
    holdType = PIECE_NONE;
    bagIndex = -1;
    random_piece(false);
    random_piece(true);
    spawn_piece();
    // current = (Piece){ bag[bagIndex], 0, 4, 1.0, false };
    for (int y = 0; y < TETRIS_BOARD_H; ++y) {
        for (int x = 0; x < TETRIS_BOARD_W; ++x) {
            board[y][x] = 0;
        }
    }
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
    if (!current.onGround) current.y += G[level - 1];
    else {
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
    int oriRot = rp.rotation;
    if (clockwise) rp.rotation = (rp.rotation + 1) % 4;
    else rp.rotation = (rp.rotation + 3) % 4;
    int newRot = rp.rotation;

    if (rp.type == PIECE_O) return rp; // O 不需要位移測試
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
    for (int i = 0; i < 7; ++i) {
        bag[index + i] = (PieceType) i;
    }

    // 洗牌
    for (int i = 0; i < 7; ++i) {
        int j = rand() % 7;
        PieceType temp = bag[index + i];
        bag[index + i] = bag[index + j];
        bag[index + j] = temp;
    }
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