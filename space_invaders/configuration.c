#include "configuration.h"

tGameConfig initializeGameConfig() {
	tGameConfig gameConfig;
	RECT initRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	gameConfig.windowRct = initRect;
	gameConfig.btnRestart = NULL;
	gameConfig.btnUpgradeBulletSize = NULL;
	gameConfig.btnUpgradeBulletSpeed = NULL;
	gameConfig.player;
	gameConfig.objectsArray = NULL;
	gameConfig.isGameRunning = TRUE;
	gameConfig.isStartMenu = TRUE;
	gameConfig.needNewGame = FALSE;
	gameConfig.upgradeInProgress = FALSE;
	gameConfig.backgroundDC = NULL;
	gameConfig.backgroundBitmap = NULL;
	gameConfig.spaceShipDC = NULL;
	gameConfig.spaceShipBitmap = NULL;
	gameConfig.gameState = END;
	gameConfig.score = 0;
	gameConfig.objectCount = 0;

	return gameConfig;
}

TVariablesConfig initializeVariableConfig() {
	TVariablesConfig variableConfig;

	variableConfig.playerSpeed = DEFAULT_PLAYER_SPEED;
	variableConfig.enemySpeed = DEFAULT_ENEMY_SPEED;
	variableConfig.bulletSize = DEFAULT_BULLET_SIZE;
	variableConfig.bulletSpeed = DEFAULT_BULLET_SPEED;

	return variableConfig;
}
