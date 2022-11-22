#pragma once

#include "core.h"

void winMove(TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig) {
	playerControl(gGameConfig, gVariablesConfig);
	movePlayer(&gGameConfig->player);
	generateEnemy(gGameConfig, gVariablesConfig);
	for (int i = 0; i < gGameConfig->objectCount; i++) {
		moveObjects(gGameConfig->objectsArray + i, gGameConfig);
		if (gGameConfig->objectsArray[i].oType == 'e' && objectCollision(gGameConfig->player, gGameConfig->objectsArray[i])) {
			gGameConfig->gameState = END;
		}
	}

	delObjects(gGameConfig);
}


void gameInit(HWND hWnd, TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig) {
	//gGameConfig = initializeGameConfig();
	//gVariablesConfig = initializeVariableConfig();

	gVariablesConfig->enemySpeed = DEFAULT_ENEMY_SPEED;
	gVariablesConfig->playerSpeed = DEFAULT_PLAYER_SPEED;
	gVariablesConfig->bulletSize = DEFAULT_BULLET_SIZE;
	gVariablesConfig->bulletSpeed = DEFAULT_BULLET_SPEED;
	gGameConfig->needNewGame = FALSE;
	gGameConfig->objectsArray = NULL;
	gGameConfig->objectCount = 0;
	KillTimer(hWnd, 1);
	gGameConfig->btnRestart = NULL;
	gGameConfig->score = 0;
	SetTimer(hWnd, 1, DIFFICULTY_INCREASE_TIME, (TIMERPROC)NULL);
	realloc(gGameConfig->objectsArray, 0);
	initObject(&gGameConfig->player, WINDOW_WIDTH / 2 - PLAYER_SIZE / 2, WINDOW_HEIGHT - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, 'p', point(0, 0));
}


void updateWindow(HDC dc, TGameConfig* gGameConfig) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP gameWindowBitmap = CreateCompatibleBitmap(dc, gGameConfig->windowRct.right - gGameConfig->windowRct.left, gGameConfig->windowRct.bottom - gGameConfig->windowRct.top);
	SelectObject(memDC, gameWindowBitmap);

	BitBlt(memDC, 0, 0, gGameConfig->windowRct.right - gGameConfig->windowRct.left, gGameConfig->windowRct.bottom - gGameConfig->windowRct.top, gGameConfig->backgroundDC, 0, 0, SRCCOPY);

	showObject(gGameConfig->player, memDC, gGameConfig);

	for (int i = 0; i < gGameConfig->objectCount; i++) {
		showObject(gGameConfig->objectsArray[i], memDC, gGameConfig);
	}

	// Copy from Memory Device Context to Device Context
	BitBlt(dc, 0, 0, gGameConfig->windowRct.right - gGameConfig->windowRct.left, gGameConfig->windowRct.bottom - gGameConfig->windowRct.top, memDC, 0, 0, SRCCOPY);

	DeleteDC(memDC);
	DeleteObject(gameWindowBitmap);
}

void loadBitmap(HDC dc, HDC* imageDc, LPCSTR bmpPath) {
	*imageDc = CreateCompatibleDC(dc);
	HBITMAP imageBitmap = (HBITMAP)LoadImageW(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(*imageDc, imageBitmap);
}

void loadImageBitmaps(HDC dc, TGameConfig* gGameConfig) {
	loadBitmap(dc, &gGameConfig->backgroundDC, L"bg-desert.bmp");
	loadBitmap(dc, &gGameConfig->spaceShipDC, L"player-spaceship.bmp");
}

void createStartButton(HWND hwnd, TGameConfig* gGameConfig) {
	if (!gGameConfig->btnRestart) {
		LPCSTR btnName = L"";

		if (gGameConfig->isStartMenu) {
			btnName = L"Start";
			gGameConfig->isStartMenu = FALSE;
		}
		else {
			btnName = L"Restart";
		}

		gGameConfig->btnRestart = CreateWindowW(L"Button", btnName, WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH / 2, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_START, NULL, NULL);
	}
}

void createUpgradeButton(HWND hwnd, TGameConfig* gGameConfig) {
	if (!gGameConfig->btnUpgradeBulletSize && !gGameConfig->btnUpgradeBulletSpeed) {
		gGameConfig->btnUpgradeBulletSize = CreateWindowW(L"Button", L"Bullet size", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH - 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SIZE, NULL, NULL);
		gGameConfig->btnUpgradeBulletSpeed = CreateWindowW(L"Button", L"Bullet speed", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SPEED, NULL, NULL);
	}
}

void destroyUpgradeButtons(TGameConfig* gGameConfig) {
	if (gGameConfig->btnUpgradeBulletSize) DestroyWindow(gGameConfig->btnUpgradeBulletSize);
	if (gGameConfig->btnUpgradeBulletSpeed) DestroyWindow(gGameConfig->btnUpgradeBulletSpeed);
	gGameConfig->btnUpgradeBulletSize = NULL;
	gGameConfig->btnUpgradeBulletSpeed = NULL;
}