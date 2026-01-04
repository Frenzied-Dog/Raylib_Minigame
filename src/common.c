#include "common.h"
#include "raylib.h"


void fixWindowDPI(int baseWidth, int baseHeight) {
	Vector2 DPI = GetWindowScaleDPI();
	if (GetRenderWidth() != baseWidth * DPI.x || GetRenderHeight() != baseHeight * DPI.y) {
		SetWindowSize(baseWidth * DPI.x, baseHeight * DPI.y);
		SetMouseScale(1.0f / DPI.x, 1.0f / DPI.y);
	}
}