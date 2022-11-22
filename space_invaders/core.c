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
