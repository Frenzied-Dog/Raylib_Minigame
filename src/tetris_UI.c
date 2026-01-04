#include <stdlib.h> /* 亂數相關函數 */
#include <time.h>   /* 時間相關函數 */

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include "tetris.h"
#include "tetris_UI.h"

static Rectangle leftCol, boardPanel, rightCol;

void DrawPiecePreview(PieceType type, Rectangle box) {
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
		DrawRectangleLinesEx((Rectangle) { px - 1, py, size + 1, size + 1 }, 1, BLACK);
	}
}

void Draw_UI(const PieceType holdType, const bool holdLocked, const int score, const int level,
			const PieceType* bag, const int bagIndex, bool *pause, const bool gameOver) {
	const int GAP = 20;
	const int PAD = 10;

	// ---------------- Left column ----------------

	Rectangle holdPanel = (Rectangle){ leftCol.x, leftCol.y, leftCol.width, 125 };
	Rectangle controlPanel = (Rectangle){ leftCol.x, leftCol.y + leftCol.height - 230, leftCol.width, 230 };

	GuiGroupBox(holdPanel, "Hold");
	{
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
		int y = (int)(controlPanel.y + 20);

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

		DrawText("Esc:", x, y + 6 * lh, fs, text);
		GuiDrawIcon(132, x + 85, y + 6 * lh - 6, 2, text);

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
	Rectangle gearBtn = { (float)(TETRIS_WINDOW_WIDTH - 20 - 50), (float)(TETRIS_WINDOW_HEIGHT - 20 - 50), (float)50, (float)50 };

	if (gameOver || *pause) {
		int prev = GuiGetState();
		GuiSetState(STATE_DISABLED);
		GuiButton(gearBtn, "#142#");
		GuiSetState(prev);
	} else if (GuiButton(gearBtn, "#142#")) {
		*pause = !(*pause);
	}
}

void Draw_Board(const int board[22][10], const Piece current, const Piece shadow) {

	// 保持方格為正方形
	int cellW = (int)(boardPanel.width / (float)TETRIS_BOARD_W);
	int cellH = (int)(boardPanel.height / (float)TETRIS_BOARD_H);
	int cell = (cellW < cellH) ? cellW : cellH;
	cell = max(cell, 1);

	int gridW = cell * TETRIS_BOARD_W;
	int gridH = cell * TETRIS_BOARD_H;
	int ox = (int)(boardPanel.x + (boardPanel.width - (float)gridW) * 0.5f);
	int oy = (int)(boardPanel.y + (boardPanel.height - (float)gridH) * 0.5f);

	Color line = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));

	// 格子區外框
	DrawRectangleLinesEx((Rectangle) { ox - 1, oy, gridW + 1, gridH + 1 }, 1, line);
	DrawLineEx((Vector2) { ox - 2, oy + cell * 2 }, (Vector2) { ox - 2, oy + gridH + 2 }, 2, line);
	DrawLineEx((Vector2) { ox + gridW + 1, oy + cell * 2 }, (Vector2) { ox + gridW + 1, oy + gridH + 2 }, 2, line);
	DrawLineEx((Vector2) { ox - 2, oy + gridH + 2 }, (Vector2) { ox + gridW + 1, oy + gridH + 2 }, 2, line);

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
			idx = (int) Clamp((float)idx, 0.0f, 6.0f);

			int px = ox + x * cell;
			int py = oy + y * cell;
			DrawRectangle(px - 1, py, cell, cell, pieceColors[idx]);
			DrawRectangleLinesEx((Rectangle) { px - 1, py, cell + 1, cell + 1 }, 1, BLACK);
		}
	}
}

int Draw_PauseScreen(TetrisState *state) {
	// 半透明遮罩 + 中央面板（繼續 / 重新開始 / 回到主選單）
	DrawRectangle(0, 0, TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT, Fade(BLACK, 0.45f));

	const int panelW = 320;
	const int panelH = 240;
	Rectangle panel = (Rectangle){
		(float)(TETRIS_WINDOW_WIDTH / 2 - panelW / 2),
		(float)(TETRIS_WINDOW_HEIGHT / 2 - panelH / 2),
		(float)panelW,
		(float)panelH
	};

	GuiPanel(panel, "Paused");

	float bx = panel.x + 40;
	float bw = panel.width - 80;
	float by = panel.y + 55;
	float bh = 36;
	float sp = 22;

	int ret = -1;
	int prevFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	if (GuiButton((Rectangle) { bx, by + 0 * (bh + sp), bw, bh }, "Continue"))
		ret = 0;
	if (GuiButton((Rectangle) { bx, by + 1 * (bh + sp), bw, bh }, "Restart"))
		ret = 1;
	if (GuiButton((Rectangle) { bx, by + 2 * (bh + sp), bw, bh }, "Main Menu"))
		ret = 2;
	GuiSetStyle(DEFAULT, TEXT_SIZE, prevFontSize);
	return ret;
}

void UI_SetLayout() {
	SetWindowSize(TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT);

	// 介面構想圖：外距與各區塊間距皆為 20
	const int M = 20;
	const int GAP = 20;
	const int LEFT_W = 220;
	const int RIGHT_W = 220;

	float innerH = (float)TETRIS_WINDOW_HEIGHT - 2.0f * M;

	leftCol = (Rectangle) { (float)M, (float)M, (float)LEFT_W, innerH };
	rightCol = (Rectangle) { (float)(TETRIS_WINDOW_WIDTH - M - RIGHT_W), (float)M, (float)RIGHT_W, innerH };
	boardPanel = (Rectangle) {
		leftCol.x + leftCol.width + (float)GAP,
		(float)M - 2,
		(float)(TETRIS_WINDOW_WIDTH - (2 * M + LEFT_W + RIGHT_W + 2 * GAP)),
		innerH + 2
	};
}

static inline Color ColorDim(Color c, float k) {
	// k in [0..1], scale RGB
	Color r = c;
	r.r = (unsigned char)((float)r.r * k);
	r.g = (unsigned char)((float)r.g * k);
	r.b = (unsigned char)((float)r.b * k);
	return r;
}

static inline Color ColorAdd(Color c, int add) {
	int rr = (int)c.r + add;
	int gg = (int)c.g + add;
	int bb = (int)c.b + add;
	rr = (int) Clamp((float)rr, 0.0f, 255.0f);
	gg = (int) Clamp((float)gg, 0.0f, 255.0f);
	bb = (int) Clamp((float)bb, 0.0f, 255.0f);
	return (Color) { (unsigned char)rr, (unsigned char)gg, (unsigned char)bb, c.a };
}

static void DrawBeveledBlock(int x, int y, int size, Color base) {
	DrawRectangle(x, y, size, size, base);

	// Bevel: top/left highlight, bottom/right shadow
	Color hi = ColorAdd(base, 35);
	Color lo = ColorAdd(base, -35);

	int t = 3; // bevel thickness
	DrawRectangle(x, y, size, t, hi);
	DrawRectangle(x, y, t, size, hi);

	DrawRectangle(x, y + size - t, size, t, lo);
	DrawRectangle(x + size - t, y, t, size, lo);

	// Outer line
	DrawRectangleLinesEx((Rectangle){x, y, size, size}, 1, ColorAdd(base, -60));
}

static void DrawGridLines(int w, int h) {
	Color c = (Color){ 18, 20, 26, 96 };
	
	for (int x = 0; x <= w; x += 50)
		DrawLineEx((Vector2) {x, 0}, (Vector2){x, h}, 1, c);
	for (int y = 0; y <= h; y += 50)
		DrawLineEx((Vector2) { 0, y }, (Vector2) { w, y }, 1, c);
}

static void DrawLetterBitmap(const int bitmap[LETTER_ROWS][LETTER_COLS], int x, int y, int cell, Color c) {
	for (int r = 0; r < LETTER_ROWS; r++) {
		for (int k = 0; k < LETTER_COLS; k++) {
			if (!bitmap[r][k]) continue;
			int px = x + k * cell;
			int py = y + r * cell;
			DrawBeveledBlock(px, py, cell, c);
		}
	}
}

static void DrawWordTETRIS(int x, int y, int cell, int letterGap) {
	// Slightly muted "tetris-like" palette (less saturated)
	Color cT = (Color){ 80, 180, 210, 255 }; // muted cyan
	Color cE = (Color){ 235, 195,  80, 255 }; // brighter soft gold
	Color cT2 = (Color){ 165, 95, 185, 255 }; // muted purple
	Color cR = (Color){ 95, 195, 120, 255 }; // muted green
	Color cI = (Color){ 220, 90,  95, 255 }; // muted red
	Color cS = (Color){ 95, 120, 215, 255 }; // muted blue

	int wLetter = LETTER_COLS * cell;
	int cx = x;

	DrawLetterBitmap(L_T, cx, y, cell, cT);  cx += wLetter + letterGap;
	DrawLetterBitmap(L_E, cx, y, cell, cE);  cx += wLetter + letterGap;
	DrawLetterBitmap(L_T, cx, y, cell, cT2); cx += wLetter + letterGap;
	DrawLetterBitmap(L_R, cx, y, cell, cR);  cx += wLetter + letterGap;
	DrawLetterBitmap(L_I, cx, y, cell, cI);  cx += wLetter + letterGap;
	DrawLetterBitmap(L_S, cx, y, cell, cS);
}

// ---------------------------- Falling Background Pieces ----------------------------

static Color PieceColorMuted(int type) {
	// Muted palette aligned with classic colors
	static const Color pal[7] = {
		{  80, 180, 210, 255 }, // I
		{ 235, 195,  80, 255 }, // O
		{ 165,  95, 185, 255 }, // T
		{  95, 195, 120, 255 }, // S
		{ 220,  90,  95, 255 }, // Z
		{  95, 120, 215, 255 }, // J
		{ 230, 145,  80, 255 }, // L (muted orange)
	};

	// return pal[type % 7];
	return pieceColors[type % 7];
}

static void ResetPiece(FallingPiece* p) {
	p->type = GetRandomValue(0, 6);
	p->rot = GetRandomValue(0, 3);
	p->cell = GetRandomValue(28, 38);
	p->speed = (float)GetRandomValue(35, 90);
	p->color = PieceColorMuted(p->type);

	float x = (float)GetRandomValue(-50, TETRIS_WINDOW_WIDTH - 50);
	float y = (float)GetRandomValue(-400, -60);
	p->pos = (Vector2){ x, y };
}

static void DrawFallingPiece(const FallingPiece* p, float alpha) {
	Color c = p->color;
	c.a = (unsigned char)(255.0f * alpha);
	int cell = p->cell;

	// draw with slight spacing so it looks like separate blocks
	for (int i = 0; i < 4; i++) {
		int bx = (int)(p->pos.x + SHAPES[p->type][p->rot][i].x * cell);
		int by = (int)(p->pos.y + SHAPES[p->type][p->rot][i].y * cell);
		DrawRectangle(bx - 1, by, cell, cell, c);
		DrawRectangleLinesEx((Rectangle) { bx - 1, by, cell + 1, cell + 1 }, 1, (Color) { 0, 0, 0, (unsigned char)(140.0f * alpha) });
	}
}

int DrawMenu() {
	// background pieces
	static FallingPiece pieces[MAX_PIECE_COUNT] = { 0 };
	static bool initialized = false;
	if (!initialized) {
		for (int i = 0; i < MAX_PIECE_COUNT; i++) {
			ResetPiece(&pieces[i]);
			pieces[i].pos.y = (float)GetRandomValue(-400, TETRIS_WINDOW_HEIGHT/2);
		}
		initialized = true;
	}

	float dt = GetFrameTime();

	// Update falling pieces
	for (int i = 0; i < MAX_PIECE_COUNT; i++) {
		pieces[i].pos.y += pieces[i].speed * dt;
		if (pieces[i].pos.y > (float)TETRIS_WINDOW_HEIGHT + 120.0f)
			ResetPiece(&pieces[i]);
	}

	// ---------------- Draw ----------------
	DrawGridLines(TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT);

	// Decorative falling blocks
	for (int i = 0; i < MAX_PIECE_COUNT; i++) {
		float a = 0.30f + 0.06f * (float)((i % 3) + 1); // varied alpha
		DrawFallingPiece(&pieces[i], a);
	}

	// Title (blocky TETRIS)
	int cell = 22;
	int letterGap = 16;
	int wordW = 6 * (LETTER_COLS * cell) + 5 * letterGap;
	int startX = (TETRIS_WINDOW_WIDTH - wordW) / 2;
	int startY = 70;

	// soft glow behind title
	DrawRectangleRounded((Rectangle) { startX - 20, startY - 15, (float)wordW + 40, 7.0f * cell + 35 }, 0.18f, 12, (Color) { 0, 0, 0, 110 });
	DrawWordTETRIS(startX, startY, cell, letterGap);

	// Center panel
	Rectangle panel = { (float)(TETRIS_WINDOW_WIDTH / 2 - 210), 300, 420, 300 };
	DrawRectangleRounded(panel, 0.18f, 14, (Color) { 0, 0, 0, 110 });
	DrawRectangleRoundedLines(panel, 0.18f, 14, (Color) { 255, 255, 255, 20 });

	// Buttons
	float bx = panel.x + 60;
	float by = panel.y + 15;
	float bw = panel.width - 120;
	float bh = 56;
	float sp = 16;

	Rectangle rSingle = { bx, by + 0 * (bh + sp), bw, bh };
	Rectangle rHost = { bx, by + 1 * (bh + sp), bw, bh };
	Rectangle rJoin = { bx, by + 2 * (bh + sp), bw, bh };
	Rectangle rBack = { bx, by + 3 * (bh + sp), bw, bh };
	
	int ret = -1;
	int prevFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	if (GuiButton(rSingle, "Single Player"))
		ret = 0;

	// Host/Join disabled
	int prev = GuiGetState();
	GuiSetState(STATE_DISABLED);
	if (GuiButton(rHost, "Host Room"))
		ret = 1;
	if (GuiButton(rJoin, "Join Room"))
		ret = 2;
	GuiSetState(prev);

	if (GuiButton(rBack, "Back To Main Menu"))
		ret = 3;

	GuiSetStyle(DEFAULT, TEXT_SIZE, prevFontSize);

	return ret;
}

// Maybe can TakeScreenshot
int DrawResultsScreen(int score, int totalLinesCleared, int level) {
	// Keyboard shortcuts
	if (IsKeyPressed(KEY_ENTER)) return 0;  // Retry
	if (IsKeyPressed(KEY_ESCAPE)) return 1; // Menu

	DrawRectangle(0, 0, TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT, Fade(BLACK, 0.55f));

	Rectangle panel = (Rectangle){ (float)(TETRIS_WINDOW_WIDTH / 2 - 230), (float)(TETRIS_WINDOW_HEIGHT / 2 - 170), 460, 340 };
	GuiPanel(panel, "Result");

	int x = (int)panel.x + 32;
	int y = (int)panel.y + 52;

	Color text = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));

	DrawText("GAME OVER", x, y - 15, 30, text);

	DrawText(TextFormat("Score: %d", score), x, y + 30, 26, text);
	DrawText(TextFormat("Lines: %d", totalLinesCleared), x, y + 65, 26, text);
	DrawText(TextFormat("Level: %d", level), x, y + 100, 26, text);

	DrawText("ENTER: Retry   ESC: Menu", x, y + 135, 20, Fade(text, 0.75f));

	float bw = panel.width - 90;
	float bh = 46;
	float bx = panel.x + 45;
	float by = panel.y + panel.height - 120;

	int ret = -1;
	int prevFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	if (GuiButton((Rectangle) { bx, by, bw, bh }, "Retry")) ret = 0;
	if (GuiButton((Rectangle) { bx, by + 60, bw, bh }, "Main Menu")) ret = 1;
	if (GuiButton((Rectangle) { panel.x + panel.width - 95, panel.y + 40, 50, 50 }, "#184#")) ret = 2; // Screenshot
	GuiSetStyle(DEFAULT, TEXT_SIZE, prevFontSize);

	return ret;
}


void getBoardGrid(int* ox, int* oy, int* cell, int* gridW, int* gridH) {
	int cw = (int)(boardPanel.width / (float)TETRIS_BOARD_W);
	int ch = (int)(boardPanel.height / (float)TETRIS_BOARD_H);
	int c = (cw < ch) ? cw : ch;
	c = max(c, 1);

	int gW = c * TETRIS_BOARD_W;
	int gH = c * TETRIS_BOARD_H;

	int oxx = (int)(boardPanel.x + (boardPanel.width - (float)gW) * 0.5f);
	int oyy = (int)(boardPanel.y + (boardPanel.height - (float)gH) * 0.5f);

	if (ox) *ox = oxx;
	if (oy) *oy = oyy;
	if (cell) *cell = c;
	if (gridW) *gridW = gW;
	if (gridH) *gridH = gH;
}