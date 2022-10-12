#pragma once
#include "object.h"
#include "constants.h"


void createEnemy() {
	// Random range formula: num = (rand() % (upper – lower + 1)) + lower 
	float enemySpeed = (rand() % (ENEMY_MAX_SPEED - ENEMY_MIN_SPEED + 1)) + ENEMY_MIN_SPEED;
	float enemyWidth = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	float enemyHeight = (rand() % (ENEMY_MAX_SIZE - ENEMY_MIN_SIZE + 1) + ENEMY_MIN_SIZE);
	float startLocation = (rand() % (WINDOW_WIDTH - (int)enemyWidth + 1));
	initObject(newObject(), startLocation, -100, enemyWidth, enemyHeight, 'e', point(0, enemySpeed));
}

void moveEnemy(TObject* obj) {
	obj->pos.y += obj->speed.y;

	if (obj->pos.y > WINDOW_HEIGHT + ENEMY_MAX_SIZE + 10) {
		obj->isDel = TRUE;
	}
}

void generateEnemy() {
	int spawnRate = rand() % ENEMY_SPAWN_RATE;

	if (spawnRate == 1) {
		createEnemy();
	}
}
