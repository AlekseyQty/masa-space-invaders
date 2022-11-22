#include "object.h"
#include "static.h"
#include "configuration.h"
#include "score.h"


TPoint point(float x, float y) {
	TPoint pt;
	pt.x = x;
	pt.y = y;

	return pt;
}

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

void showObject(TObject obj, HDC dc, TGameConfig* gGameConfig) {
	SelectObject(dc, GetStockObject(DC_PEN));
	SetDCPenColor(dc, RGB(0, 0, 0));
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(dc, obj.brush);

	showScore(dc, gGameConfig);

	if (obj.oType == 'e' || obj.oType == 'b') {
		Ellipse(dc, (int)(obj.pos.x), (int)(obj.pos.y), (int)(obj.pos.x + obj.size.x), (int)(obj.pos.y + obj.size.y));
	}

	else if (obj.oType == 'p') {
		BitBlt(dc, (int)(obj.pos.x), (int)(obj.pos.y), 100, obj.size.x, gGameConfig->spaceShipDC, 0, 0, SRCCOPY);
	}
}

void moveObjects(TObject* obj, TGameConfig* gGameConfig) {
	obj->pos.y += obj->speed.y;

	if (obj->oType == 'e' && (obj->pos.y > WINDOW_HEIGHT + ENEMY_MAX_SIZE + 10)) {
		obj->isDel = TRUE;
		gGameConfig->gameState = END;
	}
	if (obj->oType == 'b' && obj->pos.y < -150) {
		obj->isDel = TRUE;
	}

	if (obj->oType == 'b') {
		for (int i = 0; i < gGameConfig->objectCount; i++) {
			if ((gGameConfig->objectsArray[i].oType == 'e') && (objectCollision(*obj, gGameConfig->objectsArray[i]))) {
				gGameConfig->objectsArray[i].isDel = TRUE;
				obj->isDel = TRUE;
				gGameConfig->score += 100;
			}
		}
	}
}


PObject newObject(TGameConfig* gGameConfig) {
	gGameConfig->objectCount++;
	gGameConfig->objectsArray = realloc(gGameConfig->objectsArray, sizeof(*gGameConfig->objectsArray) * gGameConfig->objectCount);
	return gGameConfig->objectsArray + gGameConfig->objectCount - 1;
}

void delObjects(TGameConfig* gGameConfig) {
	int i = 0;
	while (i < gGameConfig->objectCount)
	{
		if (gGameConfig->objectsArray[i].isDel) {
			gGameConfig->objectCount--;
			gGameConfig->objectsArray[i] = gGameConfig->objectsArray[gGameConfig->objectCount];
			gGameConfig->objectsArray = realloc(gGameConfig->objectsArray, sizeof(*gGameConfig->objectsArray) * gGameConfig->objectCount);
		}
		else
			i++;
	}
}
