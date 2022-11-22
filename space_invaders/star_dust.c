// space_invaders.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "resource.h"
#include <stdio.h>
#include <time.h>
#include "static.h"
#include "types.h"
#include "configuration.h"


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

TPoint point(float x, float y) {
	TPoint pt;
	pt.x = x;
	pt.y = y;

	return pt;
}


TGameConfig gGameConfig;
TVariablesConfig gVariablesConfig;

// Functionds

BOOL objectCollision(TObject o1, TObject o2) {
	return ((o1.pos.x + o1.size.x) > o2.pos.x) && (o1.pos.x < (o2.pos.x + o2.size.x)) &&
		((o1.pos.y + o1.size.y) > o2.pos.y) && (o1.pos.y < (o2.pos.y + o2.size.y));
}

void initObject(TObject* obj, float xPos, float yPos, float width, float height, char objType, TPoint speed) {
	obj->pos = point(xPos, yPos);
	obj->size = point(width, height);
	obj->speed = speed;
	obj->oType = objType;
	if (objType == 'p') obj->brush = RGB(0, 255, 0);
	if (objType == 'e') obj->brush = RGB(230, 40, 40);
	if (objType == 'b') obj->brush = RGB(146, 255, 230);
	obj->isDel = FALSE;
}

void showScore(HDC dc) {
	// Update score
	char output[100];
	snprintf(output, 100, "Score: %d", gGameConfig.score);
	SetTextColor(dc, RGB(255, 255, 255));
	SetBkMode(dc, TRANSPARENT);
	TextOutA(dc, 50, 20, output, lstrlenA(output));
}

void showObject(TObject obj, HDC dc) {
	SelectObject(dc, GetStockObject(DC_PEN));
	SetDCPenColor(dc, RGB(0, 0, 0));
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(dc, obj.brush);

	showScore(dc);

	if (obj.oType == 'e' || obj.oType == 'b') {
		Ellipse(dc, (int)(obj.pos.x), (int)(obj.pos.y), (int)(obj.pos.x + obj.size.x), (int)(obj.pos.y + obj.size.y));
	}

	else if (obj.oType == 'p') {
		BitBlt(dc, (int)(obj.pos.x), (int)(obj.pos.y), 100, obj.size.x, gGameConfig.spaceShipDC, 0, 0, SRCCOPY);
	}
}

void movePlayer(TObject* obj) {
	obj->pos.x += obj->speed.x;
	//obj->pos.y += obj->speed.y;
	if (obj->pos.x <= 0) obj->pos.x = 0;
	if (obj->pos.x + obj->size.x >= WINDOW_WIDTH) obj->pos.x = WINDOW_WIDTH - obj->size.x;
	//if (obj->pos.y <= 0) obj->pos.y = 0;
	//if (obj->pos.y + obj->size.y >= WINDOW_HEIGHT) obj->pos.y = WINDOW_HEIGHT - obj->size.y;
}

void moveObjects(TObject* obj) {
	obj->pos.y += obj->speed.y;

	if (obj->oType == 'e' && (obj->pos.y > WINDOW_HEIGHT + ENEMY_MAX_SIZE + 10)) {
		obj->isDel = TRUE;
		gGameConfig.gameState = END;
	}
	if (obj->oType == 'b' && obj->pos.y < -150) {
		obj->isDel = TRUE;
	}

	if (obj->oType == 'b') {
		for (int i = 0; i < gGameConfig.objectCount; i++) {
			if ((gGameConfig.objectsArray[i].oType == 'e') && (objectCollision(*obj, gGameConfig.objectsArray[i]))) {
				gGameConfig.objectsArray[i].isDel = TRUE;
				obj->isDel = TRUE;
				gGameConfig.score += 100;
			}
		}
	}
}


PObject newObject() {
	gGameConfig.objectCount++;
	gGameConfig.objectsArray = realloc(gGameConfig.objectsArray, sizeof(*gGameConfig.objectsArray) * gGameConfig.objectCount);
	return gGameConfig.objectsArray + gGameConfig.objectCount - 1;
}

void delObjects() {
	int i = 0;
	while (i < gGameConfig.objectCount)
	{
		if (gGameConfig.objectsArray[i].isDel) {
			gGameConfig.objectCount--;
			gGameConfig.objectsArray[i] = gGameConfig.objectsArray[gGameConfig.objectCount];
			gGameConfig.objectsArray = realloc(gGameConfig.objectsArray, sizeof(*gGameConfig.objectsArray) * gGameConfig.objectCount);
		}
		else
			i++;
	}
}


void addBullet(float xPos, float yPos) {
	PObject obj = newObject();
	initObject(obj, xPos + (gGameConfig.player.size.x / 2 - gVariablesConfig.bulletSize / 2), yPos, gVariablesConfig.bulletSize, gVariablesConfig.bulletSize, 'b', point(0, - gVariablesConfig.bulletSpeed));
}

void createEnemy() {
	// Random range formula: num = (rand() % (upper – lower + 1)) + lower 
	float enemySpeed = (rand() % (gVariablesConfig.enemySpeed - ENEMY_MIN_SPEED + 1)) + ENEMY_MIN_SPEED;
	float enemyWidth = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	//float enemyHeight = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	float startLocation = (rand() % (WINDOW_WIDTH - (int)enemyWidth + 1));
	initObject(newObject(), startLocation, -100, enemyWidth, enemyWidth, 'e', point(0, enemySpeed));
}

void generateEnemy() {
	int spawnRate = rand() % ENEMY_SPAWN_RATE;

	if (spawnRate == 1) {
		createEnemy();
	}
}

void gameInit(HWND hWnd) {
	//gGameConfig = initializeGameConfig();
	//gVariablesConfig = initializeVariableConfig();

	gVariablesConfig.enemySpeed = DEFAULT_ENEMY_SPEED;
	gVariablesConfig.playerSpeed = DEFAULT_PLAYER_SPEED;
	gVariablesConfig.bulletSize = DEFAULT_BULLET_SIZE;
	gVariablesConfig.bulletSpeed = DEFAULT_BULLET_SPEED;
	gGameConfig.needNewGame = FALSE;
	gGameConfig.objectsArray = NULL;
	gGameConfig.objectCount = 0;
	KillTimer(hWnd, 1);
	gGameConfig.btnRestart = NULL;
	gGameConfig.score = 0;
	SetTimer(hWnd, 1, DIFFICULTY_INCREASE_TIME, (TIMERPROC)NULL);
	realloc(gGameConfig.objectsArray, 0);
	initObject(&gGameConfig.player, WINDOW_WIDTH / 2 - PLAYER_SIZE / 2, WINDOW_HEIGHT - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, 'p', point(0, 0));
}

void playerControl() {
	gGameConfig.player.speed.x = 0;
	gGameConfig.player.speed.y = 0;

	//if (GetAsyncKeyState('W') < 0) player.speed.y -= PLAYER_SPEED;
	//if (GetAsyncKeyState('S') < 0) player.speed.y = PLAYER_SPEED;
	if (GetAsyncKeyState('A') < 0) gGameConfig.player.speed.x -= gVariablesConfig.playerSpeed;
	if (GetAsyncKeyState('D') < 0) gGameConfig.player.speed.x = gVariablesConfig.playerSpeed;


	// Code for moving diagonal
	//if ((player.speed.x != 0) && (player.speed.y != 0)) {
	//	player.speed = point(player.speed.x * 0.7, player.speed.y = player.speed.y * 0.7);
	//}
}

void winMove() {
	playerControl();
	movePlayer(&gGameConfig.player);
	generateEnemy();
	for (int i = 0; i < gGameConfig.objectCount; i++) {
		moveObjects(gGameConfig.objectsArray + i);
		if (gGameConfig.objectsArray[i].oType == 'e' && objectCollision(gGameConfig.player, gGameConfig.objectsArray[i])) {
			gGameConfig.gameState = END;
		}
	}

	delObjects();
}

void loadBitmap(HDC dc, HDC* imageDc, LPCSTR bmpPath) {
	*imageDc = CreateCompatibleDC(dc);
	HBITMAP imageBitmap = (HBITMAP)LoadImageW(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(*imageDc, imageBitmap);
}

void loadImageBitmaps(HDC dc) {
	loadBitmap(dc, &gGameConfig.backgroundDC, L"bg-desert.bmp");
	loadBitmap(dc, &gGameConfig.spaceShipDC, L"player-spaceship.bmp");
}

void upgradePlayer() {
	if (gGameConfig.score % UPGRADE_TRESHOLD == 0 && !gGameConfig.upgradeInProgress && gGameConfig.score != 0) {
		gGameConfig.gameState = UPGRADING;
		gGameConfig.upgradeInProgress = TRUE;
	}

	else if (gGameConfig.upgradeInProgress) {
		if (gGameConfig.score % UPGRADE_TRESHOLD != 0) {
			gGameConfig.upgradeInProgress = FALSE;
		}
	}
}

void updateWindow(HDC dc) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP gameWindowBitmap = CreateCompatibleBitmap(dc, gGameConfig.windowRct.right - gGameConfig.windowRct.left, gGameConfig.windowRct.bottom - gGameConfig.windowRct.top);
	SelectObject(memDC, gameWindowBitmap);

	BitBlt(memDC, 0, 0, gGameConfig.windowRct.right - gGameConfig.windowRct.left, gGameConfig.windowRct.bottom - gGameConfig.windowRct.top, gGameConfig.backgroundDC, 0, 0, SRCCOPY);

	showObject(gGameConfig.player, memDC);

	for (int i = 0; i < gGameConfig.objectCount; i++) {
		showObject(gGameConfig.objectsArray[i], memDC);
	}

	// Copy from Memory Device Context to Device Context
	BitBlt(dc, 0, 0, gGameConfig.windowRct.right - gGameConfig.windowRct.left, gGameConfig.windowRct.bottom - gGameConfig.windowRct.top, memDC, 0, 0, SRCCOPY);

	DeleteDC(memDC);
	DeleteObject(gameWindowBitmap);
}

void createStartButton(HWND hwnd) {
	if (!gGameConfig.btnRestart) {
		LPCSTR btnName = L"";

		if (gGameConfig.isStartMenu) {
			btnName = L"Start";
			gGameConfig.isStartMenu = FALSE;
		}
		else {
			btnName = L"Restart";
		}

		gGameConfig.btnRestart = CreateWindowW(L"Button", btnName, WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH / 2, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_START, NULL, NULL);
	}
}

void createUpgradeButton(HWND hwnd) {
	if (!gGameConfig.btnUpgradeBulletSize && !gGameConfig.btnUpgradeBulletSpeed) {
		gGameConfig.btnUpgradeBulletSize = CreateWindowW(L"Button", L"Bullet size", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH - 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SIZE, NULL, NULL);
		gGameConfig.btnUpgradeBulletSpeed = CreateWindowW(L"Button", L"Bullet speed", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SPEED, NULL, NULL);
	}
}

void destroyUpgradeButtons() {
	if (gGameConfig.btnUpgradeBulletSize) DestroyWindow(gGameConfig.btnUpgradeBulletSize);
	if (gGameConfig.btnUpgradeBulletSpeed) DestroyWindow(gGameConfig.btnUpgradeBulletSpeed);
	gGameConfig.btnUpgradeBulletSize = NULL;
	gGameConfig.btnUpgradeBulletSpeed = NULL;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SPACEINVADERS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	srand(time(NULL));

	HWND hWnd = InitInstance(hInstance, nCmdShow);
	// Perform application initialization:
	if (!hWnd)
	{
		return FALSE;
	}
	gameInit(hWnd);
	HDC dc = GetDC(hWnd);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPACEINVADERS));
	MSG msg;


	// Main message loop:
	
	loadImageBitmaps(dc);

	while (gGameConfig.isGameRunning)
	{
		if (PeekMessageA(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			switch (gGameConfig.gameState)
			{
			case UPGRADING:
				if (!gGameConfig.btnUpgradeBulletSize && !gGameConfig.btnUpgradeBulletSpeed) {
					updateWindow(dc);
				}
				createUpgradeButton(hWnd);
				Sleep(5);
				break;
			case END:
				if (!gGameConfig.btnRestart) {
					updateWindow(dc);
				}
				createStartButton(hWnd);
				Sleep(5);
				break;
			case RUNNING:
				if (gGameConfig.needNewGame) {
					gameInit(hWnd);
				}
				winMove();
				upgradePlayer();
				updateWindow(dc);
				Sleep(5);
				break;
			}
		}
	}
	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// setup the class instance of our Win32 application
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPACEINVADERS));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SPACEINVADERS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// attempt to register the win32 application class
	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	gGameConfig = initializeGameConfig();
	gVariablesConfig = initializeVariableConfig();

	// Adjust window due to menus/borders taking some space
	AdjustWindowRect(&gGameConfig.windowRct, WS_OVERLAPPEDWINDOW, TRUE);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		10, 10, gGameConfig.windowRct.right - gGameConfig.windowRct.left, gGameConfig.windowRct.bottom - gGameConfig.windowRct.top, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BTN_START:
			gGameConfig.needNewGame = TRUE;
			gGameConfig.gameState = RUNNING;
			if (gGameConfig.btnRestart) DestroyWindow(gGameConfig.btnRestart);
			break;
		case ID_BTN_UPGRADE_BULLET_SIZE:
			gVariablesConfig.bulletSize += UPGRADE_INCREMENT;
			destroyUpgradeButtons();
			gGameConfig.gameState = RUNNING;
			break;
		case ID_BTN_UPGRADE_BULLET_SPEED:
			gVariablesConfig.bulletSpeed += UPGRADE_INCREMENT;
			destroyUpgradeButtons();
			gGameConfig.gameState = RUNNING;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here..
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		gGameConfig.isGameRunning = FALSE;
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &gGameConfig.windowRct);
		break;
	case WM_KEYUP:
		// 32 == SPACE
		if (wParam == 32) {
			addBullet(gGameConfig.player.pos.x, gGameConfig.player.pos.y);
		}
		// 27 == ESC
		else if (wParam == 27) {
			gGameConfig.isGameRunning = FALSE;
		}
		break;
	case WM_TIMER:
		if (wParam == 1) {
			gVariablesConfig.enemySpeed++;
			gVariablesConfig.playerSpeed++;
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
