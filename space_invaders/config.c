#include "config.h"
#include "configurations.h"

TConfig gameConfigInit(HWND* hWnd) {
	TConfig gameConfig;

	gameConfig.enemySpeed = DEFAULT_ENEMY_SPEED;
	gameConfig.playerSpeed = DEFAULT_PLAYER_SPEED;
	gameConfig.bulletSize = DEFAULT_BULLET_SIZE;
	gameConfig.bulletSpeed = DEFAULT_BULLET_SPEED;
	gameConfig.needNewGame = FALSE;
	gameConfig.objectsArray = NULL;
	gameConfig.objectCount = 0;
	KillTimer(hWnd, 1);
	//btnRestart = NULL;
	gameConfig.score = 0;
	SetTimer(hWnd, 1, DIFFICULTY_INCREASE_TIME, (TIMERPROC)NULL);
	realloc(gameConfig.objectsArray, 0);
	//initObject(&player, WINDOW_WIDTH / 2 - PLAYER_SIZE / 2, WINDOW_HEIGHT - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, 'p', point(0, 0));

	return gameConfig;
}