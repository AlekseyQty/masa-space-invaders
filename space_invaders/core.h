#pragma once

#include "configuration.h"
#include "player.h"
#include "enemy.h"

void winMove(TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig);

void gameInit(HWND hWnd, TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig);

void updateWindow(HDC dc, TGameConfig* gGameConfig);

void loadBitmap(HDC dc, HDC* imageDc, LPCSTR bmpPath);

void loadImageBitmaps(HDC dc, TGameConfig* gGameConfig);

void createStartButton(HWND hwnd, TGameConfig* gGameConfig);

void createUpgradeButton(HWND hwnd, TGameConfig* gGameConfig);

void destroyUpgradeButtons(TGameConfig* gGameConfig);