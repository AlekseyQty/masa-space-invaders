#pragma once
#include "framework.h"

typedef struct sPoint {
	float x, y;
} TPoint;

typedef struct SObject {
	TPoint pos;
	TPoint size;
	COLORREF brush;
	TPoint speed;
	char oType;
	BOOL isDel;
} TObject, * PObject;

enum gameState {
	RUNNING = 0,
	PAUSE = 1,
	END = 2,
	UPGRADING = 3
};
