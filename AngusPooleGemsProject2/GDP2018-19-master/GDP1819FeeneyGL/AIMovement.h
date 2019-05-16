#pragma once

#include "globalStuff.h"
#include "cMazeMaker.h"

int getMazePosX(cMeshSceneObject* obj);
int getMazePosZ(cMeshSceneObject* obj);

int DeterminePath(cMeshSceneObject* obj, cMazeMaker* maze);
int DetermineDistance(cMeshSceneObject* obj, cMazeMaker* maze, int direction);
