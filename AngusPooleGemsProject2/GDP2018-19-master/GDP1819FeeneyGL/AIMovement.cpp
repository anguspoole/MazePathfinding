#include "AIMovement.h"
#include <time.h>

int getMazePosX(cMeshSceneObject* obj)
{
	return obj->mazeX;
}

int getMazePosZ(cMeshSceneObject* obj)
{
	return obj->mazeZ;
}

int DeterminePath(cMeshSceneObject* obj, cMazeMaker* maze)
{
	srand(time(NULL));

	int mazeX = 0;
	int mazeZ = 0;

	bool upOpen = false;
	bool downOpen = false;
	bool leftOpen = false;
	bool rightOpen = false;

	mazeX = getMazePosX(obj);
	mazeZ = getMazePosZ(obj);

	std::cout << "MazePos: " << mazeX << " , " << mazeZ << std::endl;

	if (mazeX > 0)
	{
		if (!maze->maze[mazeX - 1][mazeZ][0])
		{
			leftOpen = true;
			std::cout << "left open " << std::endl;
		}
	}

	if (mazeX < mazeWidth - 1)
	{
		if (!maze->maze[mazeX + 1][mazeZ][0])
		{
			rightOpen = true;
			std::cout << "right open " << std::endl;
		}
	}

	if (mazeZ > 0)
	{
		if (!maze->maze[mazeX][mazeZ - 1][0])
		{
			downOpen = true;
			std::cout << "down open " << std::endl;
		}
	}

	if (mazeZ < mazeHeight - 1)
	{
		if (!maze->maze[mazeX][mazeZ + 1][0])
		{
			upOpen = true;
			std::cout << "up open " << std::endl;
		}
	}

	int chosenDirection = 0;

	bool canGo = false;

	do
	{
		chosenDirection = rand() % 4; // 0 - 3

		if (chosenDirection == 0 && upOpen)
		{
			canGo = true;
			break;
		}
		else if (chosenDirection == 1 && leftOpen)
		{
			canGo = true;
			break;
		}
		else if (chosenDirection == 2 && rightOpen)
		{
			canGo = true;
			break;
		}
		else if (chosenDirection == 3 && downOpen)
		{
			canGo = true;
			break;
		}

	} while (!canGo);

	std::cout << "Chosen Direction: " << chosenDirection << std::endl;

	return chosenDirection;
}

int DetermineDistance(cMeshSceneObject* obj, cMazeMaker* maze, int direction)
{
	int mazeX = 0;
	int mazeZ = 0;

	mazeX = getMazePosX(obj);
	mazeZ = getMazePosZ(obj);

	int maxTravel = 1;

	if (direction == 0) //traveling up
	{
		while (mazeZ + maxTravel + 1 < mazeHeight)
		{
			//if we hit a wall
			if (maze->maze[mazeX][mazeZ + maxTravel + 1][0])
			{
				//maxTravel--;
				break;
			}
			else
			{
				maxTravel++;
			}
		}
	}
	else if (direction == 1) //traveling left
	{
		while (mazeX - (maxTravel + 1) > 0)
		{
			//if we hit a wall
			if (maze->maze[mazeX - (maxTravel + 1)][mazeZ][0])
			{
				break;
			}
			else
			{
				maxTravel++;
			}
		}
	}
	else if (direction == 2) //traveling right
	{
		while (mazeX + maxTravel + 1 < mazeWidth)
		{
			//if we hit a wall
			if (maze->maze[mazeX + maxTravel + 1][mazeZ][0])
			{
				break;
			}
			else
			{
				maxTravel++;
			}
		}
	}
	else if (direction == 3) //traveling down
	{
		while (mazeZ - (maxTravel + 1) > 0)
		{
			//if we hit a wall
			if (maze->maze[mazeX][mazeZ - (maxTravel + 1)][0])
			{
				break;
			}
			else
			{
				maxTravel++;
			}
		}
	}

	return maxTravel;
}