#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */
#include <stdio.h>

#include "raylib.h"
#include "raygui.h"
#include "tetris.h"

static int WINDOW_WIDTH = 850;
static int WINDOW_HEIGHT = 650;
static Rectangle leftCol, boardPanel, rightCol;
static TetrisState state = MENU;
static int aniState = 0; // TODO: animation state

static int board[TETRIS_BOARD_H][TETRIS_BOARD_W]; // 0=空，其它代表方塊種類
static Piece current = {PIECE_NONE, 0, 0, 0.0, false};
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

static const double G[] = { 0.01667, 0.021017, 0.026977, 0.035256, 0.04693, 0.06361, 0.0879, 0.1236, 0.1775, 0.2598, 0.388, 0.59, 0.92, 1.46, 2.36 }; // 等級對應重力 (幀/格)
static const Vector2 SHAPES[7][4][4] = {
    // PIECE_I
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },  // rot 0  ----
        { { 1, -1 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },  // rot 1
        { { -1, 1 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },  // rot 2
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },  // rot 3  
    },
    // PIECE_O
    {
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 } },
    },
    // PIECE_T
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 0 } },
        { { 1, 0 }, { 0, 0 }, { -1, 0 }, { 0, 1 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, 0 } },
    },
    // PIECE_S
     {
        { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 1, -1 } },
        { { 0, -1 }, { 0, 0 }, { 1, 0 }, { 1, 1 } },
        { { 1, 0 }, { 0, 0 }, { 0, 1 }, { -1, 1 } },
        { { 0, 1 }, { 0, 0 }, { -1, 0 }, { -1, -1 } },
    },
    // PIECE_Z
    {
        { { -1, -1 }, { 0, -1 }, { 0, 0 }, { 1, 0 } },
        { { 1, -1 }, { 1, 0 }, { 0, 0 }, { 0, 1 } },
        { { 1, 1 }, { 0, 1 }, { 0, 0 }, { -1, 0 } },
        { { -1, 1 }, { -1, 0 }, { 0, 0 }, { 0, -1 } },
    },
    // PIECE_J
    {
        { { -1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 } },
        { { 1, -1 }, { 0, -1 }, { 0, 0 }, { 0, 1 } },
        { { 1, 1 }, { 1, 0 }, { 0, 0 }, { -1, 0 } },
        { { -1, 1 }, { 0, 1 }, { 0, 0 }, { 0, -1 } },
    },
    // PIECE_L
    {
        { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, -1 } },
        { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } },
        { { 1, 0 }, { 0, 0 }, { -1, 0 }, { -1, 1 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, -1 } },
    },
};

// 中低彩度（比 raylib 預設柔和，但不會太灰）
static const Color pieceColors[7] = {
    (Color){ 90, 170, 210, 255}, // soft cyan/sky
    (Color){235, 200,  80, 255}, // soft gold
    (Color){175, 105, 200, 255}, // soft purple
    (Color){ 95, 195, 110, 255}, // soft green
    (Color){220,  95,  95, 255}, // soft red
    (Color){ 85, 115, 220, 255}, // soft blue
    (Color){235, 155,  75, 255}, // soft orange
};

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
static void Draw_UI();
static void Draw_Board();
static void Draw_PauseScreen();
static void DrawPiecePreview(PieceType type, Rectangle box);
static void UI_SetLayout();

void tetris(menuState *mainState) {
    Font t_font = GetFontDefault();
    t_font.baseSize = 7;
    GuiSetFont(t_font);
    UI_SetLayout();
    while (!WindowShouldClose() && *mainState == STATE_TETRIS) {
        fixWindowDPI(WINDOW_WIDTH, WINDOW_HEIGHT);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        switch(state) {
        case MENU:
            if (GuiButton((Rectangle) { 80, 90, 120, 30 }, "#191#Start Game")) {
                Tetris_Init();
                state = PLAYING;
            }
            
            if (GuiButton((Rectangle) { 80, 50, 140, 30 }, "#191#Back to Menu")) {
                *mainState = MAIN_MENU;
            }
            break;
        case PLAYING:
            Draw_Board();
            Draw_UI();

            if (!gameOver) {
                if (!pause) Tetris_Update(Tetris_GetInput());
                else Draw_PauseScreen();
            } else {
                // TODO: Animation
                state = GAMEOVER;
            }
            break;
        case GAMEOVER:
            Draw_Board();
            Draw_UI();
            DrawText("Game Over", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, 40, RED);
            // TODO: Game Over Screen
            break;
        }
        
        EndDrawing();
    }
    GuiSetFont(GetFontDefault());
}


// TODO: Implement the functions below
static void Tetris_Init() {
    srand(time(NULL));
    score = 0;
    level = 1;
    gameOver = false;
    pause = false;
    holdLocked = false;
    holdType = PIECE_NONE;
    bagIndex = -1;
    random_piece(false);
    random_piece(true);
    spawn_piece();
    current = (Piece){ bag[bagIndex], 0, 4, 1.0, false };
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
    static int totalLinesCleared = 0;

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


static void DrawPiecePreview(PieceType type, Rectangle box) {
    // DrawRectangleLinesEx(box, 1, Fade(GRAY, 0.5f));
    float cell = ((box.width < box.height ? box.width : box.height) - 20.0f) / 2.0f;
    float centerX = box.x + box.width * 0.5f;
    float centerY = box.y + box.height * 0.5f;

    // 特殊調整各種方塊的預覽位置
    switch (type) {
    case PIECE_NONE:
        GuiDrawIcon(113, (int)(box.x + box.width / 2 - 16), (int)(box.y + box.height / 2 - 16), 2, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
        return;
    case PIECE_I:
        centerX -= cell * 0.5f;
        break;
    case PIECE_O:
        centerX -= cell * 0.5f;
        centerY += cell * 0.5f;
        break;
    case PIECE_T:
    case PIECE_S:
    case PIECE_Z:
    case PIECE_J:
    case PIECE_L:
        centerY += cell * 0.5f;
        break;
    default:
        break;
    }

    for (int i = 0; i < 4; ++i) {
        int px = centerX + SHAPES[type][0][i].x * cell - cell * 0.5f;
        int py = centerY + SHAPES[type][0][i].y * cell - cell * 0.5f;
        int size = (int)(cell);

        DrawRectangle(px - 1, py, size, size, pieceColors[type]);
        DrawRectangleLinesEx((Rectangle){px - 1, py, size+1, size+1}, 1, BLACK);
    }
}

static void Draw_UI() {
    const int GAP = 20;
    const int PAD = 10;

    // ---------------- Left column ----------------

    Rectangle holdPanel = (Rectangle){ leftCol.x, leftCol.y, leftCol.width, 125 };
    Rectangle controlPanel = (Rectangle){ leftCol.x, leftCol.y + leftCol.height - 230, leftCol.width, 230 };

    GuiGroupBox(holdPanel, "Hold");
    {
        if (holdType < PIECE_I || holdType > PIECE_L) holdType = PIECE_NONE;

        Rectangle holdBox = (Rectangle){
            holdPanel.x + PAD,
            holdPanel.y + 15,
            holdPanel.width - 2 * PAD,
            holdPanel.height - 15 - PAD
        };
        DrawPiecePreview(holdType, holdBox);

        if (holdLocked) {
            Color text = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
            DrawRectangleRec(holdPanel, Fade(GRAY, 0.15f));
            GuiDrawIcon(137, holdPanel.x + holdPanel.width - 32, holdPanel.y + 2, 2, text);
        }
    }

    GuiGroupBox(controlPanel, "Control");
    {
        Color text = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
        int fs = 20;
        int lh = fs + 10;
        int x = (int)(controlPanel.x + PAD);
        int y = (int)(controlPanel.y + 30);

        // DrawText("Left/Right\nDown\nZ\nX\nC\nSpace", x, y + 0 * lh, fs, text);
        DrawText("Left/Right:     /", x, y + 0 * lh, fs, text);
        GuiDrawIcon(118, x + 135, y + 0 * lh + 2, 1, text);
        GuiDrawIcon(119, x + 175, y + 0 * lh + 2, 1, text);

        DrawText("Down:   soft drop", x, y + 1 * lh, fs, text);
        
        DrawText("Z:", x, y + 2 * lh, fs, text);
        GuiDrawIcon(72, x + 85, y + 2 * lh - 8, 2, text);
        
        DrawText("X/Up:", x, y + 3 * lh, fs, text);
        GuiDrawIcon(73, x + 85, y + 3 * lh - 6, 2, text);
        
        DrawText("C:        hold", x, y + 4 * lh, fs, text);
        DrawText("Space:  hard drop", x, y + 5 * lh, fs, text);
    }

    // ---------------- Middle column (frame only) ----------------
    DrawRectangleLinesEx(boardPanel, 2, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));

    // ---------------- Right column ----------------
    Rectangle nextPanel = (Rectangle){ rightCol.x, rightCol.y, rightCol.width, 235 };
    Rectangle scorePanel = (Rectangle){ rightCol.x, nextPanel.y + nextPanel.height + GAP, rightCol.width, 110 };
    Rectangle levelPanel = (Rectangle){ rightCol.x, scorePanel.y + scorePanel.height + GAP, rightCol.width, 110 };

    GuiGroupBox(nextPanel, "Next");
    {
        float px = nextPanel.x + PAD;
        float py = nextPanel.y + 15;
        float pw = nextPanel.width - 2 * PAD;
        float ph = 80.0f;
        float innerGap = 10.0f;

        Rectangle box1 = (Rectangle){ px, py, pw, ph };
        Rectangle box2 = (Rectangle){ px + pw * 0.1, py + box1.height - 1, pw * 0.8, ph * 0.8 };
        Rectangle box3 = (Rectangle){ px + pw * 0.1, py + box1.height + box2.height - 2, pw * 0.8, ph * 0.8 };
        // Rectangle smallBox2 = (Rectangle){ px + bigW + innerGap + smallW + innerGap, py, smallW, ph };

        DrawPiecePreview(bag[(bagIndex + 1) % 14], box1);
        DrawPiecePreview(bag[(bagIndex + 2) % 14], box2);
        DrawPiecePreview(bag[(bagIndex + 3) % 14], box3);
    }

    GuiGroupBox(scorePanel, "Score");
    {
        Color text = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
        const int fs = 40;
        const char* s = TextFormat("%d", score);
        int tw = MeasureText(s, fs);
        int tx = (int)(scorePanel.x + scorePanel.width * 0.5f - tw * 0.5f);
        int ty = (int)(scorePanel.y + scorePanel.height * 0.5f - fs * 0.5f);
        DrawText(s, tx, ty, fs, text);
    }

    GuiGroupBox(levelPanel, "Level");
    {
        Color text = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
        const int fs = 40;
        const char* s = TextFormat("%d", level);
        int tw = MeasureText(s, fs);
        int tx = (int)(levelPanel.x + levelPanel.width * 0.5f - tw * 0.5f);
        int ty = (int)(levelPanel.y + levelPanel.height * 0.5f - fs * 0.5f);
        DrawText(s, tx, ty, fs, text);
    }

    // 齒輪按鈕：右下角，距離視窗邊界 20
    Rectangle gearBtn = { (float)(WINDOW_WIDTH - 20 - 50), (float)(WINDOW_HEIGHT - 20 - 50), (float)50, (float)50 };
    if (GuiButton(gearBtn, "#142#")) {
        pause = !pause;
    }

    // // 目前你的主迴圈暫停繪製被註解掉了；先在這裡直接疊上去，確保按齒輪會看到暫停畫面
    // if (pause) Draw_PauseScreen();
}

static void Draw_Board() {

    // 保持方格為正方形
    int cellW = (int)(boardPanel.width / (float)TETRIS_BOARD_W);
    int cellH = (int)(boardPanel.height / (float)TETRIS_BOARD_H);
    int cell = (cellW < cellH) ? cellW : cellH;
    if (cell < 1) cell = 1;

    int gridW = cell * TETRIS_BOARD_W;
    int gridH = cell * TETRIS_BOARD_H;
    int ox = (int)(boardPanel.x + (boardPanel.width - (float)gridW) * 0.5f);
    int oy = (int)(boardPanel.y + (boardPanel.height - (float)gridH) * 0.5f);

    Color line = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
    
    // 格子區外框
    DrawRectangleLinesEx((Rectangle) { ox - 1, oy, gridW + 1, gridH + 1 }, 0.8, line);
    DrawLineEx((Vector2) { ox - 2, oy + cell * 2 }, (Vector2) { ox - 2, oy + gridH + 2 }, 1.5, line);
    DrawLineEx((Vector2) { ox + gridW + 1, oy + cell * 2 }, (Vector2) { ox + gridW + 1, oy + gridH + 2 }, 1.5, line);
    DrawLineEx((Vector2) { ox - 2, oy + gridH + 2 }, (Vector2) { ox + gridW + 1, oy + gridH + 2 }, 1.5, line);

    // 輕量格線（用 style 的 line color 淡化）
    for (int x = 1; x < TETRIS_BOARD_W; ++x) {
        DrawLine(ox + x * cell, oy, ox + x * cell, oy + gridH, Fade(line, 0.25f));
    }
    for (int y = 1; y < TETRIS_BOARD_H; ++y) {
        DrawLine(ox, oy + y * cell, ox + gridW, oy + y * cell, Fade(line, 0.25f));
    }

    // 當前方塊（框架：邏輯補完後會自然呈現）
    if (current.type >= PIECE_I && current.type <= PIECE_L) {
        for (int i = 0; i < 4; ++i) {
            int bx = current.x + (int)SHAPES[current.type][current.rotation][i].x;
            int by = (int)current.y + (int)SHAPES[current.type][current.rotation][i].y;

            if (bx < 0 || bx >= TETRIS_BOARD_W) continue;
            if (by < 0 || by >= TETRIS_BOARD_H) continue;

            int px = ox + bx * cell;
            int py = oy + by * cell;
            DrawRectangle(px - 1, py, cell, cell, pieceColors[current.type]);
            DrawRectangleLinesEx((Rectangle) { px - 1, py, cell + 1, cell + 1 }, 1, BLACK);
        }

        // 陰影方塊
        Piece shadow = current;
        while (true) {
            shadow.y += 1.0;
            if (check_collision(&shadow)) {
                shadow.y -= 1.0;
                break;
            }
        }

        for (int i = 0; i < 4; ++i) {
            int bx = shadow.x + (int)SHAPES[shadow.type][shadow.rotation][i].x;
            int by = (int)shadow.y + (int)SHAPES[shadow.type][shadow.rotation][i].y;

            if (bx < 0 || bx >= TETRIS_BOARD_W) continue;
            if (by < 0 || by >= TETRIS_BOARD_H) continue;

            int px = ox + bx * cell;
            int py = oy + by * cell;
            DrawRectangle(px - 1, py, cell, cell, Fade(pieceColors[shadow.type], 0.4f));
            DrawRectangleLinesEx((Rectangle) { px - 1, py, cell + 1, cell + 1 }, 1, Fade(BLACK, 0.25f));
        }
    }

    // 已鎖定在盤面的方塊（board[y][x]：0=空，其它=種類）
    for (int y = 0; y < TETRIS_BOARD_H; ++y) {
        for (int x = 0; x < TETRIS_BOARD_W; ++x) {
            int v = board[y][x];
            if (v <= 0) continue;

            // 預設以 1..7 存（所以 -1 取色），不符合就 clamp
            int idx = v - 1;
            if (idx < 0) idx = 0;
            if (idx > 6) idx = 6;

            int px = ox + x * cell;
            int py = oy + y * cell;
            DrawRectangle(px - 1, py, cell, cell, pieceColors[idx]);
            DrawRectangleLinesEx((Rectangle) { px - 1, py, cell + 1, cell + 1 }, 1, BLACK);
        }
    }
}

static void Draw_PauseScreen() {
    // 半透明遮罩 + 中央面板（繼續 / 重新開始 / 回到主選單）
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.45f));

    const int panelW = 320;
    const int panelH = 240;
    Rectangle panel = (Rectangle){
        (float)(WINDOW_WIDTH / 2 - panelW / 2),
        (float)(WINDOW_HEIGHT / 2 - panelH / 2),
        (float)panelW,
        (float)panelH
    };

    GuiPanel(panel, "Paused");

    float bx = panel.x + 40;
    float bw = panel.width - 80;
    float by = panel.y + 55;
    float bh = 36;
    float sp = 22;

    if (GuiButton((Rectangle) { bx, by + 0 * (bh + sp), bw, bh }, "Continue")) {
        pause = false;
    }
    if (GuiButton((Rectangle) { bx, by + 1 * (bh + sp), bw, bh }, "Restart")) {
        pause = false;
        Tetris_Init();
        state = PLAYING;
    }
    if (GuiButton((Rectangle) { bx, by + 2 * (bh + sp), bw, bh }, "Main Menu")) {
        pause = false;
        state = MENU;
    }
}

static void UI_SetLayout() {
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    Vector2 pos = GetWindowPosition();
    SetWindowPosition(pos.x-100, pos.y-100);
    // 介面構想圖：視窗 900x600；外距與各區塊間距皆為 20
    const int M = 20;
    const int GAP = 20;
    const int LEFT_W = 220;
    const int RIGHT_W = 220;

    float innerH = (float)WINDOW_HEIGHT - 2.0f * M;

    leftCol = (Rectangle){ (float)M, (float)M, (float)LEFT_W, innerH };
    rightCol = (Rectangle){ (float)(WINDOW_WIDTH - M - RIGHT_W), (float)M, (float)RIGHT_W, innerH };
    boardPanel = (Rectangle){
        leftCol.x + leftCol.width + (float)GAP,
        (float) M-2,
        (float)(WINDOW_WIDTH - (2 * M + LEFT_W + RIGHT_W + 2 * GAP)),
        innerH+2
    };
}