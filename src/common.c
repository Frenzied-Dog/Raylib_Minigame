#include "common.h"
#include "raylib.h"


void fixWindowDPI(int baseWidth, int baseHeight) {
	Vector2 DPI = GetWindowScaleDPI();
	if (GetRenderWidth() != baseWidth * DPI.x || GetRenderHeight() != baseHeight * DPI.y) {
		SetWindowSize(baseWidth * DPI.x, baseHeight * DPI.y);
		SetMouseScale(1.0f / DPI.x, 1.0f / DPI.y);
	}
}

float Randf(float a, float b) {
	// raylib GetRandomValue is inclusive, we map to float range
	int r = GetRandomValue(0, 10000);
	float t = (float)r / 10000.0f;
	return a + (b - a) * t;
}