#pragma once
#include "cMeshSceneObject.h"
#include "cMazeMaker.h"

struct sMazeNode
{
	int x = 0;
	int z = 0;
	int cost = INT_MAX;
};

extern std::string getMazeKey(int x, int z);
extern sMazeNode* AStar(cMeshSceneObject* obj, cMazeMaker* maze, int finalX, int finalZ);

