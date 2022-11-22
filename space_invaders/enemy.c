#pragma once

#include "enemy.h"

void generateEnemy(TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig) {
	int spawnRate = rand() % ENEMY_SPAWN_RATE;

	if (spawnRate == 1) {
		float enemySpeed = (rand() % (gVariablesConfig->enemySpeed - ENEMY_MIN_SPEED + 1)) + ENEMY_MIN_SPEED;
		float enemyWidth = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
		float startLocation = (rand() % (WINDOW_WIDTH - (int)enemyWidth + 1));
		initObject(newObject(gGameConfig), startLocation, -100, enemyWidth, enemyWidth, 'e', point(0, enemySpeed));
	}
}