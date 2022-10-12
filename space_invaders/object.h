#pragma once
#include "framework.h"

typedef struct sPoint {
	float x, y;
} TPoint;

TPoint point(float x, float y) {
	TPoint pt;
	pt.x = x;
	pt.y = y;

	return pt;
}

typedef struct SObject {
	TPoint pos;
	TPoint size;
	COLORREF brush;
	TPoint speed;
	char oType;
	BOOL isDel;
} TObject, * PObject;


void initObject(TObject* obj, float xPos, float yPos, float width, float height, char objType, TPoint speed) {
	obj->pos = point(xPos, yPos);
	obj->size = point(width, height);
	obj->speed = speed;
	obj->oType = objType;
	if (objType == 'p') obj->brush = RGB(0, 255, 0);
	if (objType == 'e') obj->brush = RGB(255, 0, 0);
	obj->isDel = FALSE;
}


void showObject(TObject obj, HDC dc) {
	SelectObject(dc, GetStockObject(DC_PEN));
	SetDCPenColor(dc, RGB(0, 0, 0));
	SelectObject(dc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(dc, obj.brush);

	Rectangle(dc, (int)(obj.pos.x), (int)(obj.pos.y), (int)(obj.pos.x + obj.size.x), (int)(obj.pos.y + obj.size.y));
}

PObject newObject(PObject mas, int* objectCount) {
	objectCount++;
	realloc(mas, sizeof(*mas) * *objectCount);
	return mas + *objectCount - 1;
}


// TODO: Move from object
void updateScore(HDC dc, int score) {
	// Update score
	char output[10];
	snprintf(output, 10, "%d", score);
	TextOutA(dc, 50, 20, output, lstrlenA(output));
}

void delObjects() {
	int i = 0;
	while (i < objectCount)
	{
		if (mas[i].isDel) {
			objectCount--;
			mas[i] = mas[objectCount];
			mas = realloc(mas, sizeof(*mas) * objectCount);
			score++;
		}
		else
			i++;
	}
}