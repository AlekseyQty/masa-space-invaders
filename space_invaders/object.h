#pragma once

#include "framework.h"
#include "types.h"
#include "configuration.h"

void initObject(TObject* obj, float xPos, float yPos, float width, float height, char objType, TPoint speed);

BOOL objectCollision(TObject o1, TObject o2);

void showObject(TObject obj, HDC dc, TGameConfig* gGameConfig);

void moveObjects(TObject* obj, TGameConfig* gGameConfig);

PObject newObject(TGameConfig* gGameConfig);

void delObjects(TGameConfig* gGameConfig);

TPoint point(float x, float y);