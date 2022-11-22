// space_invaders.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "resource.h"

#include <time.h>
#include "static.h"
#include "types.h"
#include "configuration.h"
#include "object.h"
#include "player.h"
#include "enemy.h"
#include "core.h"

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

TGameConfig gGameConfig;
TVariablesConfig gVariablesConfig;

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
	gameInit(hWnd, &gGameConfig, &gVariablesConfig);
	HDC dc = GetDC(hWnd);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPACEINVADERS));
	MSG msg;


	// Main message loop:
	
	loadImageBitmaps(dc, &gGameConfig);

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
					updateWindow(dc, &gGameConfig);
				}
				createUpgradeButton(hWnd, &gGameConfig);
				Sleep(5);
				break;
			case END:
				if (!gGameConfig.btnRestart) {
					updateWindow(dc, &gGameConfig);
				}
				createStartButton(hWnd, &gGameConfig);
				Sleep(5);
				break;
			case RUNNING:
				if (gGameConfig.needNewGame) {
					gameInit(hWnd, &gGameConfig, &gVariablesConfig);
				}
				winMove(&gGameConfig, &gVariablesConfig);
				upgradePlayer(&gGameConfig);
				updateWindow(dc, &gGameConfig);
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
			destroyUpgradeButtons(&gGameConfig);
			gGameConfig.gameState = RUNNING;
			break;
		case ID_BTN_UPGRADE_BULLET_SPEED:
			gVariablesConfig.bulletSpeed += UPGRADE_INCREMENT;
			destroyUpgradeButtons(&gGameConfig);
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
			addBullet(gGameConfig.player.pos.x, gGameConfig.player.pos.y, &gGameConfig, &gVariablesConfig);
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
