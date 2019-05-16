#include "globalStuff.h"
#include "cMazeMaker.h"
#include "Pathfinding.h"
#include <iostream>
#include <math.h>
#include <algorithm>

std::string getMazeKey(int x, int z)
{
	std::string key = "x" + std::to_string(x) + "z" + std::to_string(z);
	return key;
}

float calculateHeuristic(int startX, int startZ, int endX, int endZ)
{
	int dx = (int)glm::abs(endX - startX);
	int dz = (int)glm::abs(endZ - startZ);

	return (dx + dz) + (-1) * std::min(dx, dz);
}

//Function for AStar Algorithm
sMazeNode* AStar(cMeshSceneObject* obj, cMazeMaker* maze, int finalX, int finalZ)
{
	//Create the object's current node
	sMazeNode* startNode = new sMazeNode();
	startNode->x = obj->mazeX;
	startNode->z = obj->mazeZ;
	startNode->cost = 0;

	if (obj->mazeX == finalX && obj->mazeZ == finalZ)
	{
		return startNode;
	}

	//Get the key for the current object
	std::string startKey = getMazeKey(obj->mazeX, obj->mazeZ);

	//std::vector<cMapNode*> closedSet; //set of nodes already dealt with
	//The sets rely on a map of strings, using: xXINTzZINT as the format
	std::map<std::string, sMazeNode*> closedSet;
	std::map<std::string, sMazeNode*> openSet;
	openSet.insert(std::pair<std::string, sMazeNode*>(startKey, startNode));

	//path map holds pointers to previous nodes, using the key for that node
	std::map<std::string, sMazeNode*> path;

	//gCosts map holds the costs to reach each node, using the key for that node
	std::map<std::string, int> gCosts;

	for (int i = 0; i < maze->maze.size(); i++)
	{
		for (int j = 0; j < maze->maze[i].size(); j++)
		{
			//Set initial costs to "infinity"
			std::string key = getMazeKey(obj->mazeX, obj->mazeZ);
			gCosts.insert(std::pair<std::string, int>(key, INT_MAX));
		}
	}

	//fCosts map holds the total costs to reach goal through that node, using the key for that node
	std::map<std::string, int> fCosts = gCosts; //assign infinity to all values

	gCosts[startKey] = 0; //set initial cost for starting node to be 0
	fCosts[startKey] = calculateHeuristic(obj->mazeX, obj->mazeZ, finalX, finalZ); //set initial total cost from start to goal as the distance between them

	sMazeNode* currentNode = NULL;

	//loop until open set is empty
	while (!openSet.empty())
	{
		std::string lowestKey;
		int lowestCost = INT_MAX;

		std::map<std::string, int>::iterator it;
		for (it = fCosts.begin(); it != fCosts.end(); it++)
		{
			if (it->second < lowestCost)
			{
				if (openSet.find(it->first) != openSet.end())
				{
					lowestKey = it->first;
					lowestCost = it->second;
				}
			}
		}

		currentNode = openSet[lowestKey];
		openSet.erase(lowestKey);
		closedSet.insert(std::pair<std::string, sMazeNode*>(lowestKey, currentNode));

		if (currentNode->x == finalX && currentNode->z == finalZ)
		{
			break; //exit loop
		}

		//Check NN neighbour of current node
		if (currentNode->z+1 < mazeHeight - 1)
		{
			bool bestPath = true;
			int tempCost = INT_MAX;
			std::string currentKey = getMazeKey(currentNode->x, currentNode->z);
			std::string northKey = getMazeKey(currentNode->x, currentNode->z+1);
			if (closedSet.find(northKey) == closedSet.end())
			{
				//if ((gCosts[currentNode->key] < INT_MAX) && (currentNode->neighbourNN->nodeColour != '_'))
				if (gCosts[currentKey] < INT_MAX)
				{
					tempCost = gCosts[currentKey] + 2;
				}

				//if not in open set, add it
				if (openSet.find(northKey) == openSet.end())
				{
					sMazeNode * northNeighbour = new sMazeNode();
					northNeighbour->x = currentNode->x;
					northNeighbour->z = currentNode->z + 1;
					openSet.insert(std::pair<std::string, sMazeNode*>(northKey, northNeighbour));
					northNeighbour = NULL;
					delete northNeighbour;
				}
				//otherwise 
				else if (tempCost >= gCosts[northKey])
				{
					bestPath = false;
				}
			}
			else
			{
				bestPath = false;
			}
			//If this is the best path
			if (bestPath)
			{
				if (path.find(northKey) == path.end())
				{
					path.insert(std::pair<std::string, sMazeNode*>(northKey, currentNode));
				}
				else
				{
					path[northKey] = currentNode;
				}
				gCosts[northKey] = tempCost;
				if (gCosts[northKey] < INT_MAX && (maze->maze[currentNode->x][currentNode->z+1][0] == false))
				{
					fCosts[northKey] = gCosts[northKey] + calculateHeuristic(currentNode->x, currentNode->z, currentNode->x, currentNode->z + 1);
				}
			}
		}

		//Check SS neighbour of current node
		if (currentNode->z - 1 > 0)
		{
			bool bestPath = true;
			int tempCost = INT_MAX;
			std::string currentKey = getMazeKey(currentNode->x, currentNode->z);
			std::string southKey = getMazeKey(currentNode->x, currentNode->z - 1);
			if (closedSet.find(southKey) == closedSet.end())
			{
				//if ((gCosts[currentNode->key] < INT_MAX) && (currentNode->neighbourNN->nodeColour != '_'))
				if (gCosts[currentKey] < INT_MAX)
				{
					tempCost = gCosts[currentKey] + 2;
				}

				//if not in open set, add it
				if (openSet.find(southKey) == openSet.end())
				{
					sMazeNode * southNeighbour = new sMazeNode();
					southNeighbour->x = currentNode->x;
					southNeighbour->z = currentNode->z - 1;
					openSet.insert(std::pair<std::string, sMazeNode*>(southKey, southNeighbour));
					southNeighbour = NULL;
					delete southNeighbour;
				}
				//otherwise 
				else if (tempCost >= gCosts[southKey])
				{
					bestPath = false;
				}
			}
			else
			{
				bestPath = false;
			}
			//If this is the best path
			if (bestPath)
			{
				if (path.find(southKey) == path.end())
				{
					path.insert(std::pair<std::string, sMazeNode*>(southKey, currentNode));
				}
				else
				{
					path[southKey] = currentNode;
				}
				gCosts[southKey] = tempCost;
				if (gCosts[southKey] < INT_MAX && (maze->maze[currentNode->x][currentNode->z-1][0] == false))
				{
					fCosts[southKey] = gCosts[southKey] + calculateHeuristic(currentNode->x, currentNode->z, currentNode->x, currentNode->z - 1);
				}
			}
		}

		//Check WW neighbour of current node
		if (currentNode->x - 1 > 0)
		{
			bool bestPath = true;
			int tempCost = INT_MAX;
			std::string currentKey = getMazeKey(currentNode->x, currentNode->z);
			std::string westKey = getMazeKey(currentNode->x - 1, currentNode->z);
			if (closedSet.find(westKey) == closedSet.end())
			{
				//if ((gCosts[currentNode->key] < INT_MAX) && (currentNode->neighbourNN->nodeColour != '_'))
				if (gCosts[currentKey] < INT_MAX)
				{
					tempCost = gCosts[currentKey] + 2;
				}

				//if not in open set, add it
				if (openSet.find(westKey) == openSet.end())
				{
					sMazeNode * westNeighbour = new sMazeNode();
					westNeighbour->x = currentNode->x - 1;
					westNeighbour->z = currentNode->z;
					openSet.insert(std::pair<std::string, sMazeNode*>(westKey, westNeighbour));
					westNeighbour = NULL;
					delete westNeighbour;
				}
				//otherwise 
				else if (tempCost >= gCosts[westKey])
				{
					bestPath = false;
				}
			}
			else
			{
				bestPath = false;
			}
			//If this is the best path
			if (bestPath)
			{
				if (path.find(westKey) == path.end())
				{
					path.insert(std::pair<std::string, sMazeNode*>(westKey, currentNode));
				}
				else
				{
					path[westKey] = currentNode;
				}
				gCosts[westKey] = tempCost;
				if (gCosts[westKey] < INT_MAX && (maze->maze[currentNode->x - 1][currentNode->z][0] == false))
				{
					fCosts[westKey] = gCosts[westKey] + calculateHeuristic(currentNode->x, currentNode->z, currentNode->x - 1, currentNode->z);
				}
			}
		}

		//Check EE neighbour of current node
		if (currentNode->x + 1 < mazeWidth - 1)
		{
			bool bestPath = true;
			int tempCost = INT_MAX;
			std::string currentKey = getMazeKey(currentNode->x, currentNode->z);
			std::string eastKey = getMazeKey(currentNode->x + 1, currentNode->z);
			if (closedSet.find(eastKey) == closedSet.end())
			{
				//if ((gCosts[currentNode->key] < INT_MAX) && (currentNode->neighbourNN->nodeColour != '_'))
				if (gCosts[currentKey] < INT_MAX)
				{
					tempCost = gCosts[currentKey] + 2;
				}

				//if not in open set, add it
				if (openSet.find(eastKey) == openSet.end())
				{
					sMazeNode * eastNeighbour = new sMazeNode();
					eastNeighbour->x = currentNode->x + 1;
					eastNeighbour->z = currentNode->z;
					openSet.insert(std::pair<std::string, sMazeNode*>(eastKey, eastNeighbour));
					eastNeighbour = NULL;
					delete eastNeighbour;
				}
				//otherwise 
				else if (tempCost >= gCosts[eastKey])
				{
					bestPath = false;
				}
			}
			else
			{
				bestPath = false;
			}
			//If this is the best path
			if (bestPath)
			{
				if (path.find(eastKey) == path.end())
				{
					path.insert(std::pair<std::string, sMazeNode*>(eastKey, currentNode));
				}
				else
				{
					path[eastKey] = currentNode;
				}
				gCosts[eastKey] = tempCost;
				if (gCosts[eastKey] < INT_MAX && (maze->maze[currentNode->x + 1][currentNode->z][0] == false))
				{
					fCosts[eastKey] = gCosts[eastKey] + calculateHeuristic(currentNode->x, currentNode->z, currentNode->x + 1, currentNode->z);
				}
			}
		}

		//end of while loop
	}

	//Create the final path
	//std::vector<sMazeNode*> finalPath;

	//finalPath.push_back(currentNode);
	std::string currentKey = getMazeKey(currentNode->x, currentNode->z);
	while (path[currentKey] != NULL)
	{
		if (path[getMazeKey(path[currentKey]->x, path[currentKey]->z)] == NULL)
		{
			break;
		}
		else
		{
			//finalPath.push_back(path[currentKey]);
			currentNode = path[currentKey];
			currentKey = getMazeKey(currentNode->x, currentNode->z);
		}
	}
	//std::reverse(finalPath.begin(), finalPath.end());
	//finalPath.erase(finalPath.begin());

	//cMeshObject* gatherer = findObjectByFriendlyName("Gatherer");
	//obj->needPath = false;
	//obj->atTarget = true;

	openSet.clear();
	closedSet.clear();
	fCosts.clear();
	gCosts.clear();
	startNode = NULL;
	delete startNode;

	//return finalPath;
	return currentNode;
}