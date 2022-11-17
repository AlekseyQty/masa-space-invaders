// space_invaders.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "resource.h"
#include <stdio.h>
#include <time.h>
#include "configurations.h"
#include "types.h"

#include "config.h"

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

// Declarations
RECT windowRct = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
HWND btnRestart = NULL;
HWND btnUpgradeBulletSize = NULL;
HWND btnUpgradeBulletSpeed = NULL;
TObject player;
PObject objectsArray = NULL;
BOOL isGameRunning = TRUE;
BOOL isStartMenu = TRUE;
BOOL needNewGame = FALSE;
BOOL upgradeInProgress = FALSE;
HDC backgroundDC = NULL;
HBITMAP backgroundBitmap = NULL;
HDC spaceShipDC = NULL;
HBITMAP spaceShipBitmap = NULL;

int objectCount;
int score;
int gEnemySpeed;
int gBulletSize;
int gBulletSpeed;
int gPlayerSpeed;
enum gameState gGameState = END;


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
	snprintf(output, 100, "Score: %d", score);
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
		BitBlt(dc, (int)(obj.pos.x), (int)(obj.pos.y), 100, obj.size.x, spaceShipDC, 0, 0, SRCCOPY);
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
		gGameState = END;
	}
	if (obj->oType == 'b' && obj->pos.y < -150) {
		obj->isDel = TRUE;
	}

	if (obj->oType == 'b') {
		for (int i = 0; i < objectCount; i++) {
			if ((objectsArray[i].oType == 'e') && (objectCollision(*obj, objectsArray[i]))) {
				objectsArray[i].isDel = TRUE;
				obj->isDel = TRUE;
				score += 100;
			}
		}
	}
}


PObject newObject() {
	objectCount++;
	objectsArray = realloc(objectsArray, sizeof(*objectsArray) * objectCount);
	return objectsArray + objectCount - 1;
}

void delObjects() {
	int i = 0;
	while (i < objectCount)
	{
		if (objectsArray[i].isDel) {
			objectCount--;
			objectsArray[i] = objectsArray[objectCount];
			objectsArray = realloc(objectsArray, sizeof(*objectsArray) * objectCount);
		}
		else
			i++;
	}
}


void addBullet(float xPos, float yPos) {
	PObject obj = newObject();
	initObject(obj, xPos + (player.size.x / 2 - gBulletSize / 2), yPos, gBulletSize, gBulletSize, 'b', point(0, -gBulletSpeed));
}

void createEnemy(TConfig* gameConfig) {
	// Random range formula: num = (rand() % (upper � lower + 1)) + lower 
	float enemySpeed = (rand() % (gameConfig->enemySpeed - ENEMY_MIN_SPEED + 1)) + ENEMY_MIN_SPEED;
	float enemyWidth = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	//float enemyHeight = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	float startLocation = (rand() % (WINDOW_WIDTH - (int)enemyWidth + 1));
	initObject(newObject(), startLocation, -100, enemyWidth, enemyWidth, 'e', point(0, enemySpeed));
}

void generateEnemy(TConfig* gameConfig) {
	int spawnRate = rand() % ENEMY_SPAWN_RATE;

	if (spawnRate == 1) {
		createEnemy(gameConfig);
	}
}

TConfig gameInit(HWND hWnd) {
	TConfig gameConfig = gameConfigInit(&hWnd);
	gEnemySpeed = DEFAULT_ENEMY_SPEED;
	gPlayerSpeed = DEFAULT_PLAYER_SPEED;
	gBulletSize = DEFAULT_BULLET_SIZE;
	gBulletSpeed = DEFAULT_BULLET_SPEED;
	needNewGame = FALSE;
	objectsArray = NULL;
	objectCount = 0;
	KillTimer(hWnd, 1);
	btnRestart = NULL;
	score = 0;
	SetTimer(hWnd, 1, DIFFICULTY_INCREASE_TIME, (TIMERPROC)NULL);
	realloc(objectsArray, 0);
	initObject(&player, WINDOW_WIDTH / 2 - PLAYER_SIZE / 2, WINDOW_HEIGHT - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, 'p', point(0, 0));
	return gameConfig;
}

void playerControl(TConfig* gameConfig) {
	player.speed.x = 0;
	player.speed.y = 0;

	//if (GetAsyncKeyState('W') < 0) player.speed.y -= PLAYER_SPEED;
	//if (GetAsyncKeyState('S') < 0) player.speed.y = PLAYER_SPEED;
	if (GetAsyncKeyState('A') < 0) player.speed.x -= gameConfig->playerSpeed;
	if (GetAsyncKeyState('D') < 0) player.speed.x = gameConfig->playerSpeed;

	// Code for moving diagonal
	//if ((player.speed.x != 0) && (player.speed.y != 0)) {
	//	player.speed = point(player.speed.x * 0.7, player.speed.y = player.speed.y * 0.7);
	//}
}

void winMove(TConfig* gameConfig) {
	playerControl(gameConfig);
	movePlayer(&player);
	generateEnemy(gameConfig);
	for (int i = 0; i < objectCount; i++) {
		moveObjects(objectsArray + i);
		if (objectsArray[i].oType == 'e' && objectCollision(player, objectsArray[i])) {
			gGameState = END;
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
	loadBitmap(dc, &backgroundDC, L"bg-desert.bmp");
	loadBitmap(dc, &spaceShipDC, L"player-spaceship.bmp");
}

void upgradePlayer() {
	if (score % UPGRADE_TRESHOLD == 0 && !upgradeInProgress && score != 0) {
		gGameState = UPGRADING;
		upgradeInProgress = TRUE;
	}

	else if (upgradeInProgress) {
		if (score % UPGRADE_TRESHOLD != 0) {
			upgradeInProgress = FALSE;
		}
	}
}

void updateWindow(HDC dc) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP gameWindowBitmap = CreateCompatibleBitmap(dc, windowRct.right - windowRct.left, windowRct.bottom - windowRct.top);
	SelectObject(memDC, gameWindowBitmap);

	BitBlt(memDC, 0, 0, windowRct.right - windowRct.left, windowRct.bottom - windowRct.top, backgroundDC, 0, 0, SRCCOPY);

	showObject(player, memDC);

	for (int i = 0; i < objectCount; i++) {
		showObject(objectsArray[i], memDC);
	}

	// Copy from Memory Device Context to Device Context
	BitBlt(dc, 0, 0, windowRct.right - windowRct.left, windowRct.bottom - windowRct.top, memDC, 0, 0, SRCCOPY);

	DeleteDC(memDC);
	DeleteObject(gameWindowBitmap);
}

void createStartButton(HWND hwnd) {
	if (!btnRestart) {
		LPCSTR btnName = L"";

		if (isStartMenu) {
			btnName = L"Start";
			isStartMenu = FALSE;
		}
		else {
			btnName = L"Restart";
		}

		btnRestart = CreateWindowW(L"Button", btnName, WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH / 2, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_START, NULL, NULL);
	}
}

void createUpgradeButton(HWND hwnd) {
	if (!btnUpgradeBulletSize && !btnUpgradeBulletSpeed) {
		btnUpgradeBulletSize = CreateWindowW(L"Button", L"Bullet size", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 - BTN_WIDTH - 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SIZE, NULL, NULL);
		btnUpgradeBulletSpeed = CreateWindowW(L"Button", L"Bullet speed", WS_VISIBLE | WS_CHILD, WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, hwnd, ID_BTN_UPGRADE_BULLET_SPEED, NULL, NULL);
	}
}

void destroyUpgradeButtons() {
	if (btnUpgradeBulletSize) DestroyWindow(btnUpgradeBulletSize);
	if (btnUpgradeBulletSpeed) DestroyWindow(btnUpgradeBulletSpeed);
	btnUpgradeBulletSize = NULL;
	btnUpgradeBulletSpeed = NULL;
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

	HDC dc = GetDC(hWnd);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPACEINVADERS));
	MSG msg;


	// Main message loop:
	TConfig gameConfig = gameInit(hWnd);
	loadImageBitmaps(dc);

	while (isGameRunning)
	{
		if (PeekMessageA(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			switch (gGameState)
			{
			case UPGRADING:
				if (!btnUpgradeBulletSize && !btnUpgradeBulletSpeed) {
					updateWindow(dc);
				}
				createUpgradeButton(hWnd);
				Sleep(5);
				break;
			case END:
				if (!btnRestart) {
					updateWindow(dc);
				}
				createStartButton(hWnd);
				Sleep(5);
				break;
			case RUNNING:
				if (needNewGame) {
					gameInit(hWnd);
				}
				winMove(&gameConfig);
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

	// Adjust window due to menus/borders taking some space
	AdjustWindowRect(&windowRct, WS_OVERLAPPEDWINDOW, TRUE);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		10, 10, windowRct.right - windowRct.left, windowRct.bottom - windowRct.top, NULL, NULL, hInstance, NULL);

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
			needNewGame = TRUE;
			gGameState = RUNNING;
			if (btnRestart) DestroyWindow(btnRestart);
			break;
		case ID_BTN_UPGRADE_BULLET_SIZE:
			gBulletSize += UPGRADE_INCREMENT;
			destroyUpgradeButtons();
			gGameState = RUNNING;
			break;
		case ID_BTN_UPGRADE_BULLET_SPEED:
			gBulletSpeed += UPGRADE_INCREMENT;
			destroyUpgradeButtons();
			gGameState = RUNNING;
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
		isGameRunning = FALSE;
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &windowRct);
		break;
	case WM_KEYUP:
		// 32 == SPACE
		if (wParam == 32) {
			addBullet(player.pos.x, player.pos.y);
		}
		// 27 == ESC
		else if (wParam == 27) {
			isGameRunning = FALSE;
		}
		break;
	case WM_TIMER:
		if (wParam == 1) {
			gEnemySpeed++;
			gPlayerSpeed++;
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