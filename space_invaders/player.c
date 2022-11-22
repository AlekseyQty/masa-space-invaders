#include "player.h"

void movePlayer(TObject* obj) {
	obj->pos.x += obj->speed.x;

	if (obj->pos.x <= 0) obj->pos.x = 0;
	if (obj->pos.x + obj->size.x >= WINDOW_WIDTH) obj->pos.x = WINDOW_WIDTH - obj->size.x;
}

void playerControl(TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig) {
	gGameConfig->player.speed.x = 0;
	gGameConfig->player.speed.y = 0;

	if (GetAsyncKeyState('A') < 0) gGameConfig->player.speed.x -= gVariablesConfig->playerSpeed;
	if (GetAsyncKeyState('D') < 0) gGameConfig->player.speed.x = gVariablesConfig->playerSpeed;
}

void addBullet(float xPos, float yPos, TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig) {
	PObject obj = newObject(gGameConfig);
	initObject(obj, xPos + (gGameConfig->player.size.x / 2 - gVariablesConfig->bulletSize / 2), yPos, gVariablesConfig->bulletSize, gVariablesConfig->bulletSize, 'b', point(0, -gVariablesConfig->bulletSpeed));
}
