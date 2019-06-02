# MazePathfinding
Pathfinding in a maze using threads

By: Angus Poole

## Video
https://www.youtube.com/watch?v=a1p1C_qsK8k

## About
This program demonstrates threading in 2 areas: the loading of models, and pathfinding for the Daleks in the maze. I initially set it up with the pathfinding, but it doesn't speed the program up fast enough for the requirement, so I then set it up with the loading of models.

The loading is split into two functions in loadModels.cpp: LoadModelTypes() and LoadModelTypes_ASYNC(). The ASYNC function will first add each model to a vector of model/shaderID pairs within the mesh manager, and then call (on line 1033) the new m_LoadModelFromFile_ASYNC() in cVAOMeshManager (line 398 of cVAOMeshManager.cpp).

A number of threads are then created and looped over in this m_LoadModelFromFile_ASYNC() function, and this thread runs the m_LoadModelFromFile_Ply5nLoader_ASYNC() function (since the models are all ply models). The critical section locking/unlocking is done entirely in this function.

Once all threads have finished running, the m_LoadMeshInfo_Into_VAO() function is called. This comes after the threads because it interacts with the shader - when inside the thread, it doesn't work properly.

Although a bunch of models are "loaded", most of them are not being drawn. The only reason there's a large number of models is for the purpose of demonstrating the thread vs no-thread model loading.

## Instructions
Build/Run in Release/x64 mode.

In theMain.cpp, line 53, the boolean variable mazeProcessingThreads can be set to true or false before running the program. If set to true, the program will load models with threads, and the pathfinding will run with threads. If set to false, the program will load models without threads, and the pathfinding won't use threads. Pressing Tab will toggle the variable, but this only affects the pathfinding.

Use WASDQE to move the camera.

Hold Alt and use WASD to move the "player character" around. This can be a bit finnicky because the player isn't allowed to pass through walls, which can make it feel a bit "sticky" if inadvertantly trying to pass through one, especially with the model animation running.