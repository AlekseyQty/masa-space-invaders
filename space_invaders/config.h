#pragma once
#include "types.h"

typedef struct SConfig {
	int enemySpeed;
	int playerSpeed;
	int bulletSize;
	int bulletSpeed;
	BOOL needNewGame;
	PObject objectsArray;
	int objectCount;
	int score;
} TConfig;

TConfig gameConfigInit(HWND* hWnd);