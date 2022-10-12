#pragma once
#include "object.h"
#include "constants.h"

void movePlayer(TObject* obj) {
	obj->pos.x += obj->speed.x;
	obj->pos.y += obj->speed.y;
	if (obj->pos.x <= 0) obj->pos.x = 0;
	if (obj->pos.x + obj->size.x >= WINDOW_WIDTH) obj->pos.x = WINDOW_WIDTH - obj->size.x;
	if (obj->pos.y <= 0) obj->pos.y = 0;
	if (obj->pos.y + obj->size.y >= WINDOW_HEIGHT) obj->pos.y = WINDOW_HEIGHT - obj->size.y;
}
