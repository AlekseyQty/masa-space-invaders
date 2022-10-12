#include "framework.h"
#include "resource.h"
#include "object.h"
#include "helpers.h"
#include "player.h"
#include "enemy.h"
#include "constants.h"
#include "stdio.h"
#include <time.h>

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Declarations
RECT rct;
RECT windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
TObject player;
PObject obj_array = NULL;
BOOL isGameRunning = TRUE;
int objectCount = 0;
int score = 0;


// Functionds



void gameInit() {
	initObject(&player, 100, 100, 40, 40, 'p', point(0,0));
	createEnemy(&obj_array, &objectCount, sizeof(*obj_array));
	createEnemy(&obj_array, &objectCount, sizeof(*obj_array));
}

void playerControl() {
	player.speed.x = 0;
	player.speed.y = 0;

	if (GetAsyncKeyState('W') < 0) player.speed.y -= PLAYER_SPEED;
	if (GetAsyncKeyState('S') < 0) player.speed.y = PLAYER_SPEED;
	if (GetAsyncKeyState('A') < 0) player.speed.x -= PLAYER_SPEED;
	if (GetAsyncKeyState('D') < 0) player.speed.x = PLAYER_SPEED;
	if ((player.speed.x != 0) && (player.speed.y != 0)) {
		player.speed = point(player.speed.x * 0.7, player.speed.y = player.speed.y * 0.7);
	}
}

void winMove() {
	playerControl();
	movePlayer(&player);
	//generateEnemy(&obj_array, &objectCount, sizeof(*obj_array));
	for (int i = 0; i < objectCount; i++) {
		moveEnemy(obj_array + i);
		//if (objectCollision(player, obj_array[i])) {
		//	obj_array[i].isDel = TRUE;
		//}
	}

	//delObjects(&mas, &objectCount);
}

void updateWindow(HDC dc) {
	HDC memDC = CreateCompatibleDC(dc);
	HBITMAP memBM = CreateCompatibleBitmap(dc, rct.right - rct.left, rct.bottom - rct.top);
	SelectObject(memDC, memBM);

	SelectObject(memDC, CreateSolidBrush(RGB(255, 255, 255)));
	Rectangle(memDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	showObject(player, memDC);
	updateScore(memDC, score);

	for (int i = 0; i < objectCount; i++) {
		showObject(obj_array[i], memDC);
	}

	// Copy from Memory Device Context to Device Context
	BitBlt(dc, 0, 0, rct.right - rct.left, rct.bottom - rct.top, memDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
	DeleteObject(memBM);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

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
	OutputDebugString(L"GAME INIT\n");
	//initPlayer(dc);
	gameInit();

	while (isGameRunning)
	{
		if (PeekMessageA(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			if (GetKeyState(VK_ESCAPE) < 0) {
				isGameRunning = FALSE;
			}
			winMove();
			updateWindow(dc);
			Sleep(5);
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
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, TRUE);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		10, 10, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, NULL);

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
		GetClientRect(hWnd, &rct);
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
