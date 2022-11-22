#include "score.h"
#include <stdio.h>


void showScore(HDC dc, TGameConfig* gGameConfig) {
	// Update score
	char output[100];
	snprintf(output, 100, "Score: %d", gGameConfig->score);
	SetTextColor(dc, RGB(255, 255, 255));
	SetBkMode(dc, TRANSPARENT);
	TextOutA(dc, 50, 20, output, lstrlenA(output));
}