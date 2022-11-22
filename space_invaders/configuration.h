#pragma once

#include "framework.h"
#include "static.h"
#include "types.h"

typedef struct SGameConfig {
	RECT windowRct;
	HWND btnRestart;
	HWND btnUpgradeBulletSize;
	HWND btnUpgradeBulletSpeed;
	TObject player;
	PObject objectsArray;
	BOOL isGameRunning;
	BOOL isStartMenu;
	BOOL needNewGame;
	BOOL upgradeInProgress;
	HDC backgroundDC;
	HBITMAP backgroundBitmap;
	HDC spaceShipDC;
	HBITMAP spaceShipBitmap;
	enum gameState gameState;
	int objectCount;
	int score;
} tGameConfig;

typedef struct SVariablesConfig {
	int enemySpeed;
	int bulletSize;
	int bulletSpeed;
	int playerSpeed;
} TVariablesConfig;


