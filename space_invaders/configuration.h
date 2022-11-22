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
	HDC spaceShipDC;
	enum gameState gameState;
	int objectCount;
	int score;
} TGameConfig;

typedef struct SVariablesConfig {
	int enemySpeed;
	int bulletSize;
	int bulletSpeed;
	int playerSpeed;
} TVariablesConfig;


TGameConfig initializeGameConfig();
TVariablesConfig initializeVariableConfig();