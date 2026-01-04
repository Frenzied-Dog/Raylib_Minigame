#include "raylib.h"
#include "raygui.h"
#include "common.h"
#include "menu.h"

// 主選單視窗大小（可自行調整）
static const int MENU_WIDTH = 850;
static const int MENU_HEIGHT = 650;

static void InitDeco(DecoBlock* b, int w, int h);
static void UpdateDeco(DecoBlock* b, float dt, int w, int h);
static void DrawDeco(const DecoBlock* b);
static void DrawGridLine(int w, int h);

void menu(menuState* mainState) {
	SetWindowSize(MENU_WIDTH, MENU_HEIGHT);

	int prevTextSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int prevBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
	GuiSetStyle(BUTTON, BORDER_WIDTH, 2);

	// 裝飾方塊初始化
	const int DECO_N = 20;
	DecoBlock deco[DECO_N];
	for (int i = 0; i < DECO_N; i++)
		InitDeco(&deco[i], MENU_WIDTH, MENU_HEIGHT);

	while (!WindowShouldClose() && *mainState == MAIN_MENU) {
		fixWindowDPI(MENU_WIDTH, MENU_HEIGHT);

		float dt = GetFrameTime();
		for (int i = 0; i < DECO_N; i++) UpdateDeco(&deco[i], dt, MENU_WIDTH, MENU_HEIGHT);

		BeginDrawing();

		// 背景（亮色）
		ClearBackground((Color) { 242, 244, 248, 255 });
		DrawGridLine(MENU_WIDTH, MENU_HEIGHT);
		for (int i = 0; i < DECO_N; i++) DrawDeco(&deco[i]);

		// 標題
		const char* title = "RETRO ARCADE";
		int titleSize = 46;
		int tw = MeasureText(title, titleSize);
		DrawText(title, MENU_WIDTH / 2 - tw / 2, 70, titleSize, (Color) { 24, 28, 36, 255 });
		DrawText("Choose a game",
			MENU_WIDTH / 2 - MeasureText("Choose a game", 22) / 2,
			130, 22, (Color) { 24, 28, 36, 170 });

		// 中央面板
		Rectangle panel = (Rectangle){ (float)(MENU_WIDTH / 2 - 240), 175, 480, 400 };
		DrawRectangleRounded(panel, 0.16f, 14, (Color) { 255, 255, 255, 210 });
		DrawRectangleRoundedLines(panel, 0.16f, 14, (Color) { 0, 0, 0, 40 });

		float bx = panel.x + 70;
		float bw = panel.width - 140;
		float bh = 56;
		float gap = 18;
		float by = panel.y + 25;

		Rectangle rTetris = (Rectangle){ bx, by + 0 * (bh + gap), bw, bh };
		Rectangle rDodge = (Rectangle){ bx, by + 1 * (bh + gap), bw, bh };
		Rectangle rSnake = (Rectangle){ bx, by + 2 * (bh + gap), bw, bh };
		Rectangle rMinesweeper = (Rectangle){ bx, by + 3 * (bh + gap), bw, bh };
		Rectangle rExit = (Rectangle){ bx, by + 4 * (bh + gap), bw, bh };

		if (GuiButton(rTetris, "Tetris"))
			*mainState = STATE_TETRIS;

		if (GuiButton(rDodge, "Dodge"))
			*mainState = STATE_DODGE;

		if (GuiButton(rSnake, "Snake"))
			*mainState = STATE_SNAKE;

		if (GuiButton(rMinesweeper, "Minesweeper"))
			*mainState = STATE_MINESWEEPER;

		if (GuiButton(rExit, "Exit Application"))
			*mainState = EXIT;

		EndDrawing();
	}
	// 還原 raygui style
	GuiSetStyle(DEFAULT, TEXT_SIZE, prevTextSize);
	GuiSetStyle(BUTTON, BORDER_WIDTH, prevBorderWidth);
}

// ---------------------------- 一點點裝飾：淡淡格線 + 漂浮方塊 ----------------------------
static void InitDeco(DecoBlock* b, int w, int h) {
	static const Color pal[7] = {
		(Color) { 80, 180, 210, 255 }, // cyan
		(Color) { 235, 195,  80, 255 }, // yellow
		(Color) { 165,  95, 185, 255 }, // purple
		(Color) { 95, 195, 120, 255 }, // green
		(Color) { 220,  90,  95, 255 }, // red
		(Color) { 95, 120, 215, 255 }, // blue
		(Color) { 230, 145,  80, 255 }, // orange
	};

	b->pos = (Vector2){ Randf(-60.0f, (float)w + 60.0f), Randf(-80.0f, (float)h + 80.0f) };
	b->vel = (Vector2){ Randf(-20.0f, 20.0f), Randf(18.0f, 60.0f) };
	b->size = Randf(16.0f, 36.0f);
	b->alpha = Randf(0.16f, 0.28f);
	b->col = pal[GetRandomValue(0, 6)];
}

static void UpdateDeco(DecoBlock* b, float dt, int w, int h) {
	b->pos.x += b->vel.x * dt;
	b->pos.y += b->vel.y * dt;

	// wrap
	if (b->pos.y > (float)h + 80.0f) {
		b->pos.y = -80.0f;
		b->pos.x = Randf(-60.0f, (float)w + 60.0f);
		b->vel.y = Randf(18.0f, 60.0f);
	}
	if (b->pos.x < -100.0f) b->pos.x = (float)w + 100.0f;
	if (b->pos.x > (float)w + 100.0f) b->pos.x = -100.0f;
}

static void DrawGridLine(int w, int h) {
	Color c = (Color){ 0, 0, 0, 18 };
	for (int x = 0; x <= w; x += 50) DrawLine(x, 0, x, h, c);
	for (int y = 0; y <= h; y += 50) DrawLine(0, y, w, y, c);
}

static void DrawDeco(const DecoBlock* b) {
	Color fill = b->col;
	fill.a = (unsigned char)(255.0f * b->alpha);

	int x = (int)b->pos.x;
	int y = (int)b->pos.y;
	int s = (int)b->size;

	DrawRectangle(x, y, s, s, fill);
	DrawRectangleLines(x, y, s, s, (Color) { 0, 0, 0, (unsigned char)(110.0f * b->alpha) });
}