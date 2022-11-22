#include "static.h"
#include "configuration.h"
#include "types.h"
#include "object.h"

void movePlayer(TObject* obj);

void playerControl(TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig);

void addBullet(float xPos, float yPos, TGameConfig* gGameConfig, TVariablesConfig* gVariablesConfig);

void upgradePlayer(TGameConfig* gGameConfig);