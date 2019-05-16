//     ___                 ___ _     
//    / _ \ _ __  ___ _ _ / __| |    
//   | (_) | '_ \/ -_) ' \ (_ | |__  
//    \___/| .__/\___|_||_\___|____| 
//         |_|                       
//
#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <iostream>		// cout (console out)

#include <vector>		// "smart array" dynamic array
#include <Windows.h>
#include <process.h>

//#include "cShaderManager.h"
#include "CGLShaderManager.h"
#include "cMeshSceneObject.h"
#include "cVAOMeshManager.h"

#include "cScene.h"

// Here' we include the actual class
// We will "pass around" the interface class pointer, though
#include "DebugRenderer/cDebugRenderer.h"

#include "cLightHelper.h"

// Changed because the OpenAL has a cAABB class in there, apparently (and it's causing a conflict)
#include "cAABB_centre_min.h"

#include "cDalek.h"
#include "cDalekManagerTripleBuffer.h"
#include "cRandThreaded.h"

#include "globalMaze.h"
#include "cMazeMaker.h"
#include "AIMovement.h"
#include "Pathfinding.h"

bool mazeProcessingThreads = true;

cDalekManagerTripleBuffer* g_pDalekMangerBuffer;
std::vector< cDalek* > g_vec_pDaleks;
static const unsigned int g_NUMBEROFDALEKS = 25;

//#include <Windows.h>		// For FreeConsole();

cDebugRenderer* g_pDebugRendererACTUAL = NULL;
iDebugRenderer* g_pDebugRenderer = NULL;



cSound* g_pSound = NULL;


void ShutDown(void);

void UpdateWindowTitle(GLFWwindow* window);




void DoPhysicsUpdate( double deltaTime, 
					  std::vector< cMeshSceneObject* > &vec_pObjectsToDraw );

std::vector< cMeshSceneObject* > vec_pObjectsToDraw;

// To the right, up 4.0 units, along the x axis
//glm::vec3 g_lightPos = glm::vec3( 4.0f, 4.0f, 0.0f );
//float g_lightBrightness = 400000.0f;

unsigned int numberOfObjectsToDraw = 0;

cFlyCamera* g_pFlyCamera = NULL;
cFollowCamera* g_pFollowCamera = NULL;
eCameraTypes g_CameraInUse = UNKNOWN_CAMERA;

cJoystickManager* g_pJoysticks = NULL;

//cShaderManager* pTheShaderManager = NULL;		// "Heap" variable
CGLShaderManager* pTheShaderManager = NULL;
cVAOMeshManager* g_pTheVAOMeshManager = NULL;

cBasicTextureManager* g_pTheTextureManager = NULL;


// You could also search for the model by name, 
//	rather than using a global variable like this.
cMeshSceneObject* g_pRogerRabbit = NULL;

// And example of the light
sLight* pTheOneLight = NULL;

// If true, then the "ambient brightness" spheres will be drawn
bool g_bDrawDebugLightSpheres = true;

// 
cAABBHierarchy* g_pTheTerrain = new cAABBHierarchy();


void DoPhysicsCheckpointNumberFour(double deltaTime);

// Calculates and draws collision points on the Mig-29 model
void Mig29CollisionTest(double deltaTime, GLuint shaderProgramID);


// For now, I'm doing this here, but you might want to do this
//  in the object, in the "phsyics" thing, or wherever. 
//  Or leave it here!!
//void LoadTerrainAABB(void);


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// For the particle emitter examples
void setUpParticleEmitters(void);							// in LoadModels.cpp
void updateAndDrawParticles( double deltaTime,				// in DrawObject_call.cpp
							 GLuint shaderProgramID,
							 glm::vec3 cameraEye );				

// Set up the off screen textures to draw to
//GLuint g_FBO = 0;
//GLuint g_FBO_colourTexture = 0;
//GLuint g_FBO_depthTexture = 0;
//GLint g_FBO_SizeInPixes = 512;		// = 512 the WIDTH of the framebuffer, in pixels;
cFBO* g_pFBOMain;

cFBO* g_pFBOEffectsPass;

int mazeWidth = 30;
int mazeHeight = 30;
cMazeMaker* maze;


unsigned int roundUp(unsigned int numToRound, unsigned int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
	{
        return numToRound;
	}

    return numToRound + multiple - remainder;
}

void InitSound(void);

void CreateDaleks(void);

CRITICAL_SECTION CS_Dalek_Pos;

std::map<std::string, glm::vec3> positionChanges;

void ProcessDaleks(cMeshSceneObject* obj, glm::vec3 change)
{
	EnterCriticalSection(&CS_Dalek_Pos);

	positionChanges[obj->friendlyName] = change;
	
	obj->position += positionChanges[obj->friendlyName];

	LeaveCriticalSection(&CS_Dalek_Pos);
}

void StartPath(cMeshSceneObject* pDalek0)
{
	cMeshSceneObject* player = findObjectByFriendlyName("Player1");
	sMazeNode * mazeNode = AStar(pDalek0, maze, player->mazeX, player->mazeZ);
	pDalek0->prevMazeX = getMazePosX(pDalek0);
	pDalek0->prevMazeZ = getMazePosZ(pDalek0);
	int xDiff = mazeNode->x - pDalek0->mazeX;
	int zDiff = mazeNode->z - pDalek0->mazeZ;
	if (xDiff != 0 || zDiff != 0)
	{
		ProcessDaleks(pDalek0, glm::vec3(xDiff * 0.2f, 0.0f, zDiff * 0.2f));
		if (glm::abs(pDalek0->position.z - pDalek0->mazeZ * 2.001f) > 1.5f)
		{
			if (zDiff > 0)
			{
				pDalek0->mazeZ++;
			}
			else
			{
				pDalek0->mazeZ--;
			}
		}
		if (glm::abs(pDalek0->position.x - pDalek0->mazeX * 2.001f) > 1.5f)
		{
			if (xDiff > 0)
			{
				pDalek0->mazeX++;
			}
			else
			{
				pDalek0->mazeX--;
			}
		}
	}
	//mazeNode = 0;
	//delete mazeNode;
	//LeaveCriticalSection(&CS_Dalek_Pos);
}

DWORD WINAPI FindPathToPlayer(PVOID pvParam)
{
	cMeshSceneObject* pDalek0 = (cMeshSceneObject*)pvParam;
	//EnterCriticalSection(&CS_Dalek_Pos);
	StartPath(pDalek0);
	//LeaveCriticalSection(&CS_Dalek_Pos);

	return 0;
}

int main(void)
{
	int dalekMax = 20;
	DWORD hThreadID[20] = { 0 };
	LPDWORD phThread[20] = { &(hThreadID[0]) };
	HANDLE hThreadHandle[20] = { 0 };

	InitializeCriticalSection(&CS_Dalek_Pos);

	cScene theScene;
	theScene.LoadSceneFromFile("testScene.xml");

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
	{
        exit(EXIT_FAILURE);
	}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1024, 480, "Hello there", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

	// Mouse callbacks
	glfwSetCursorPosCallback( window, cursor_position_callback );
	glfwSetMouseButtonCallback( window, mouse_button_callback );
	glfwSetCursorEnterCallback( window, cursor_enter_callback );
	glfwSetScrollCallback( window, scroll_callback );
	glfwSetJoystickCallback(joystick_callback);

			// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback( window, window_size_callback );



    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

	::g_pJoysticks = new cJoystickManager();
	::g_pJoysticks->InitOrReset();

	// Joystick info
	std::cout << "Found: " << ::g_pJoysticks->getNumberOfJoysticksPresent() << " joysticks" << std::endl;
	if ( ::g_pJoysticks->getNumberOfJoysticksPresent() > 0 )
	{
		std::cout
			<< ">"
			<< ::g_pJoysticks->getJoystickName( ::g_pJoysticks->getFirstJoystickID() )
			<< "< is the 1st joystick found." << std::endl;
		::g_pJoysticks->DetectJoystickType(::g_pJoysticks->getFirstJoystickID());
	}


//	// Bye, bye console...
//	FreeConsole();


	// Load the sound...
	InitSound();

//	::g_pSound->Play("Happy");
//	::g_pSound->Play("Sad");

	// Create the shader manager...
	pTheShaderManager = new CGLShaderManager();
	pTheShaderManager->SetBaseFilePath("assets/shaders/");

	CShaderProgramDescription basicShader;
	basicShader.name = "BasicUberShader";

	basicShader.vShader.filename = "vertex01.glsl";
	basicShader.vShader.type = GLSHADERTYPES::VERTEX_SHADER;

	//basicShader.tEvalShader.filename = "tessEval.glsl";
	//basicShader.tEvalShader.type = GLSHADERTYPES::TESSELLATION_EVALUATION_SHADER;

	//basicShader.tContShader.filename = "tessControl.glsl";
	//basicShader.tContShader.type = GLSHADERTYPES::TESSELLATION_CONTROL_SHADER;
	
//	// Geometry 
	basicShader.gShader.filename = "geomPassThrough01.glsl";
	basicShader.gShader.type = GLSHADERTYPES::GEOMETRY_SHADER;
	
	basicShader.fShader.filename = "fragment01.glsl";
	basicShader.fShader.type = GLSHADERTYPES::FRAGMENT_SHADER;

	//if ( pTheShaderManager->createProgramFromFile( "BasicUberShader", 
	//										       vertexShader, 
	//										       fragmentShader ) )
	//if ( pTheShaderManager->createProgramFromFile( "BasicUberShader", 
	//										       vertexShader, 
	//										       geomShader,
	//										       fragmentShader ) )
	//if ( pTheShaderManager->createProgramFromFile( "BasicUberShader", 
	//										       vertexShader,  
	//	                                           tessEvalShader,
	//	                                           tessControlShader,
	//										       geomShader,
	//										       fragmentShader ) )
	if ( pTheShaderManager->CreateShaderProgramFromFile( basicShader ) )
	{		// Shaders are OK
		std::cout << "Compiled shaders OK." << std::endl;
	}
	else
	{		
		std::cout << "OH NO! Compile error" << std::endl;
		std::cout << pTheShaderManager->GetLastError() << std::endl;
	}

	pTheShaderManager->LoadActiveShaderVariables("BasicUberShader");

	// Load the uniform location values (some of them, anyway)
// 	cShaderManager::cShaderProgram* pSP = ::pTheShaderManager->pGetShaderProgramFromFriendlyName("BasicUberShader");
//	pSP->LoadUniformLocation("texture00");
//	pSP->LoadUniformLocation("texture01");
//	pSP->LoadUniformLocation("texture02");
//	pSP->LoadUniformLocation("texture03");
//	pSP->LoadUniformLocation("texture04");
//	pSP->LoadUniformLocation("texture05");
//	pSP->LoadUniformLocation("texture06");
//	pSP->LoadUniformLocation("texture07");
//	pSP->LoadUniformLocation("texBlendWeights[0]");
//	pSP->LoadUniformLocation("texBlendWeights[1]");

	// Camera creation
	::g_pFlyCamera = new cFlyCamera();
	::g_pFollowCamera = new cFollowCamera();

	::g_pFlyCamera->eye = glm::vec3(mazeWidth, 20.0f, -mazeHeight);

	::g_pFollowCamera->fullSpeedDistanceFromTarget = 2.0f;
	::g_pFollowCamera->eye = glm::vec3(0.0f,0.0f,0.0f);
	::g_pFollowCamera->maxSpeed = 3.0f;

	::g_CameraInUse = /*eCameraTypes::*/FOLLOW_CAMERA;
	::g_CameraInUse = /*eCameraTypes::*/FLY_CAMERA;


//	GLuint program = pTheShaderManager->getIDFromFriendlyName("BasicUberShader");
	GLuint program = pTheShaderManager->GetShaderIDFromName("BasicUberShader");


	::g_pTheVAOMeshManager = new cVAOMeshManager();

	// Create the texture manager
	::g_pTheTextureManager = new cBasicTextureManager();



	::g_pFBOMain = new cFBO();

//
//	if ( FBOStatus == GL_FRAMEBUFFER_COMPLETE )
	std::string FBOErrorString;
	// This is a 16x9 aspect ratio
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

	if ( ::g_pFBOMain->init( fbWidth, fbHeight, FBOErrorString ) )
//	if ( ::g_pFBOMain->init( 3840, 2160, FBOErrorString ) )
//	if ( ::g_pFBOMain->init( 256, 256, FBOErrorString ) )
	{
		std::cout << "Main Framebuffer is good to go!" << std::endl;
	}
	else
	{
		std::cout << "Main Framebuffer is NOT complete" << std::endl;
	}

	::g_pFBOEffectsPass = new cFBO();
	if ( ::g_pFBOEffectsPass->init( fbWidth, fbHeight, FBOErrorString ) )
	{
		std::cout << "Effects Framebuffer is good to go!" << std::endl;
	}
	else
	{
		std::cout << "Effects Framebuffer is NOT complete" << std::endl;
	}



	// Point back to default frame buffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	

	// ****************************



	// Loading the uniform variables here (rather than the inner draw loop)
	GLint objectColour_UniLoc = glGetUniformLocation( program, "objectColour" );
	//uniform vec3 lightPos;
	//uniform float lightAtten;
	//GLint lightPos_UniLoc = glGetUniformLocation( program, "lightPos" );
	//GLint lightBrightness_UniLoc = glGetUniformLocation( program, "lightBrightness" );
	
	//	// uniform mat4 MVP;	THIS ONE IS NO LONGER USED	
	//uniform mat4 matModel;	// M
	//uniform mat4 matView;		// V
	//uniform mat4 matProj;		// P
	//GLint mvp_location = glGetUniformLocation(program, "MVP");
	GLint matModel_location = glGetUniformLocation(program, "matModel");
	GLint matView_location = glGetUniformLocation(program, "matView");
	GLint matProj_location = glGetUniformLocation(program, "matProj");

	GLint eyeLocation_location = glGetUniformLocation(program, "eyeLocation");
	
	// Note that this point is to the +interface+ but we're creating the actual object
	::g_pDebugRendererACTUAL = new cDebugRenderer();
	::g_pDebugRenderer = (iDebugRenderer*)::g_pDebugRendererACTUAL;

	if ( ! ::g_pDebugRendererACTUAL->initialize() )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRendererACTUAL->getLastError() << std::endl;
	}
	else
	{
		std::cout << "Debug renderer is OK" << std::endl;
	}


	
	// Skinned mesh
//	LoadSkinnedMeshModel( ::vec_pObjectsToDraw, program );

	std::string loadingMessage;

	if (!mazeProcessingThreads)
	{
		double startTime = glfwGetTime();
		LoadModelTypes(::g_pTheVAOMeshManager, program);
		double basicTime = glfwGetTime() - startTime;
		loadingMessage = "Without threads, loading took: " + std::to_string(basicTime) + " seconds";
	}
	else
	{
		double startTime = glfwGetTime();
		LoadModelTypes_ASYNC(::g_pTheVAOMeshManager, program);
		double complexTime = glfwGetTime() - startTime;
		loadingMessage = "With threads, loading took: " + std::to_string(complexTime) + " seconds";
	}

	LoadModelsIntoScene(::vec_pObjectsToDraw);

	maze = new cMazeMaker();
	maze->GenerateMaze(mazeWidth, mazeHeight);
	maze->PrintMaze(::vec_pObjectsToDraw);


	int dalekCount = 0;
	//int dalekMax = 20;

	for (dalekCount = 0; dalekCount < dalekMax; dalekCount++)
	{
		cMeshSceneObject* pDalek = new cMeshSceneObject();
		pDalek->position = glm::vec3(0.0f, 0.0f, 0.0f);
		//		pDebugSphere->objColour = glm::vec3( 0.0f, 1.0f, 0.0f );
		pDalek->setDiffuseColour(glm::vec3(1.0f, 1.0f, 0.0f));
		float scale = 0.035f;
		pDalek->friendlyName = "Dalek" + std::to_string(dalekCount);
		pDalek->uniformScale = scale;
		pDalek->adjMeshOrientationEulerAngles(glm::vec3(glm::radians(135.0f), 0.0f, 0.0f));

		//pDalek->meshName = "dalek2005_xyz_uv_res_2.ply";		// "Sphere_320_faces_xyz.ply";
		pDalek->vecLODMeshs.push_back(sLODInfo("dalek2005_xyz_uv_res_2.ply"));

		pDalek->bIsWireFrame = false;
		pDalek->bIsVisible = true;
		pDalek->bDontLight = false;
		pDalek->pDebugRenderer = ::g_pDebugRenderer;
		pDalek->bIsUpdatedByPhysics = true;
		vec_pObjectsToDraw.push_back(pDalek);

		bool dalekPlaced = false;
		positionChanges.insert(std::pair<std::string, glm::vec3>(pDalek->friendlyName, glm::vec3(0.0f)));

		do
		{
			int dalekX = std::rand() % (mazeWidth - 2) + 1;
			int dalekZ = std::rand() % (mazeHeight - 2) + 1;

			if (!maze->maze[dalekX][dalekZ][0])
			{
				pDalek->mazeX = dalekX;
				pDalek->mazeZ = dalekZ;
				pDalek->position = glm::vec3(dalekX * 2.001f, 0.0f, dalekZ * 2.001f);
				//pDalek->position = glm::vec3(dalekX * 2.001f, 0.0f, dalekZ * 2.001f);
				dalekPlaced = true;
				//std::cout << "Maze: " << maze->maze[dalekX][dalekZ][0] << std::endl;
				//std::cout << "Dalek: " << dalekX << " , " << dalekZ << std::endl;
			}
		} while (!dalekPlaced);
	}

	cMeshSceneObject * player1 = findObjectByFriendlyName("Player1");

	bool playerPlaced = false;

	do
	{
		int posX = std::rand() % (mazeWidth - 2) + 1;
		int posZ = std::rand() % (mazeHeight - 2) + 1;

		if (!maze->maze[posX][posZ][0])
		{
			player1->mazeX = posX;
			player1->mazeZ = posZ;
			player1->position = glm::vec3(posX * 2.001f, 0.0f, posZ * 2.001f);
			//pDalek->position = glm::vec3(dalekX * 2.001f, 0.0f, dalekZ * 2.001f);
			playerPlaced = true;
		}
	} while (!playerPlaced);

	//************************************************
	if (mazeProcessingThreads)
	{
		for (int i = 0; i < dalekMax; i++)
		{
			std::string dalekName = "Dalek" + std::to_string(i);

			cMeshSceneObject* pDalek = findObjectByFriendlyName(dalekName);

			PVOID* pDalekV = (PVOID*)pDalek;

			//Start the thread
			hThreadHandle[i] = CreateThread(NULL,
				0,
				//&MoveDalek, //the thread's function
				&FindPathToPlayer, //the thread's function
				pDalekV, //value being pased
				0,
				(DWORD*)(&(phThread[i])));
		}
	}


//	LoadTerrainAABB();

	// Get the current time to start with
	double lastTime = glfwGetTime();		



	// This would happen in the "light manager", where it would 
	//	do this for each light...
	pTheOneLight = new sLight();

	// Do this once...
	pTheOneLight->position_UniLoc = glGetUniformLocation( program, "theLights[0].position" );
	pTheOneLight->diffuse_UniLoc = glGetUniformLocation( program, "theLights[0].diffuse" );
	pTheOneLight->specular_UniLoc = glGetUniformLocation( program, "theLights[0].specular" );
	pTheOneLight->atten_UniLoc = glGetUniformLocation( program, "theLights[0].atten" );
	pTheOneLight->direction_UniLoc = glGetUniformLocation( program, "theLights[0].direction" );
	pTheOneLight->param1_UniLoc = glGetUniformLocation( program, "theLights[0].param1" );
	pTheOneLight->param2_UniLoc = glGetUniformLocation( program, "theLights[0].param2" );

//	pTheOneLight->position = glm::vec4( 0.0f, 30.0f, 0.0f, 1.0f );	
	pTheOneLight->position = glm::vec4( 124.0f, 42.0f, -46.0f, 1.0f );	
	//pTheOneLight->atten.x = 0.0f;				// 	float constAtten = 0.0f;
	//pTheOneLight->atten.y = 0.00385720730f;		//	float linearAtten = 0.01f;
	//pTheOneLight->atten.z = 0.00001f;		//	float quadAtten = 0.001f;
	pTheOneLight->atten.x = 0.0f;				// 	float constAtten = 0.0f;
	pTheOneLight->atten.y = 0.00698917499f;		//	float linearAtten = 0.01f;
	pTheOneLight->atten.z = 9.99999975e-06f;		//	float quadAtten = 0.001f;
	pTheOneLight->diffuse = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );		// White light


	pTheOneLight->SetLightType( sLight::POINT_LIGHT );


	cLightHelper* pLightHelper = new cLightHelper();


	// Threading Dalek example
	//CreateDaleks();

	int renderPassNumber = 1;
	// 1 = 1st pass (the actual scene)
	// 2 = 2nd pass (rendering what we drew to the output)
	GLint renderPassNumber_UniLoc = glGetUniformLocation( program, "renderPassNumber" );
	std::cout << renderPassNumber_UniLoc << std::endl;


	int frameCount = 0;
	double dTime = 0;
	double oldTime = 0;

	// Draw the "scene" (run the program)
	while (!glfwWindowShouldClose(window))
    {

		// High res timer (likely in ms or ns)
		double currentTime = glfwGetTime();		
		frameCount++;
		double deltaTime = currentTime - lastTime;
		dTime = currentTime - oldTime;
		if (dTime >= 1.0f)
		{
			system("CLS"); //reset console output
			std::cout << "FPS: " << frameCount << std::endl;

			if (mazeProcessingThreads)
			{
				std::cout << "Currently using threads..." << std::endl;
			}
			else
			{
				std::cout << "Curently not using threads..." << std::endl;
			}
			std::cout << loadingMessage << std::endl;

			frameCount = 0;
			oldTime = currentTime;
		}

		double MAX_DELTA_TIME = 0.1;	// 100 ms
		if ( deltaTime > MAX_DELTA_TIME)
		{
			deltaTime = MAX_DELTA_TIME;
		}


		// update the "last time"
		lastTime = currentTime;

		// The physics update loop
		DoPhysicsUpdate( deltaTime, vec_pObjectsToDraw );

		// Switch to the shader we want
		::pTheShaderManager->UseShaderProgram( "BasicUberShader", true );

		// Set for the 1st pass
		//glBindFramebuffer( GL_FRAMEBUFFER, g_FBO );		// Point output to FBO

		glBindFramebuffer( GL_FRAMEBUFFER, ::g_pFBOMain->ID );
//		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		//**********************************************************
		//     ___ _                _   _          ___ ___  ___  
		//    / __| |___ __ _ _ _  | |_| |_  ___  | __| _ )/ _ \ 
		//   | (__| / -_) _` | '_| |  _| ' \/ -_) | _|| _ \ (_) |
		//    \___|_\___\__,_|_|    \__|_||_\___| |_| |___/\___/ 
		//                                                       		
		// Clear the offscreen frame buffer
//		glViewport( 0, 0, g_FBO_SizeInPixes, g_FBO_SizeInPixes );
//		GLfloat	zero = 0.0f;
//		GLfloat one = 1.0f;
//		glClearBufferfv( GL_COLOR, 0, &zero );
//		glClearBufferfv( GL_DEPTH, 0, &one );

		// Clear colour and depth buffers
		::g_pFBOMain->clearBuffers(true, true);
		//**********************************************************


		glUniform1f( renderPassNumber_UniLoc, 1.0f );	// Tell shader it's the 1st pass


        float ratio;
        int width, height;
 		//glm::mat4x4 mvp;		
		// Now these are split into separate parts
		// (because we need to know whe
		glm::mat4x4 matProjection = glm::mat4(1.0f);
		glm::mat4x4	matView = glm::mat4(1.0f);
 

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

		//// But I'm rendering off-screen at 3840, 2160
		//ratio = 3840 / (float) 2160;
		//glViewport(0, 0, 3840, 2160);

		// These things will impact ANY framebuffer
		// (controls state of the rendering, so doesn't matter where
		//  the output goes to, right?)
		glEnable( GL_DEPTH_TEST );	// When drawing, checked the existing depth
		glEnable( GL_CULL_FACE );	// Discared "back facing" triangles

		// Colour and depth buffers are TWO DIFF THINGS.
		// Note that this is clearing the main framebuffer
		// (Which will do NOTHING to the offscreen buffer)
 //       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		matProjection = glm::perspective( 0.6f,			// FOV
			                                ratio,		// Aspect ratio
			                                0.1f,			// Near clipping plane
			                                10000.0f );	// Far clipping plane

	
		switch (::g_CameraInUse)
		{
		case eCameraTypes::FLY_CAMERA:
			glUniform3f( eyeLocation_location, ::g_pFlyCamera->eye.x, ::g_pFlyCamera->eye.y, ::g_pFlyCamera->eye.z );
			matView = glm::lookAt( ::g_pFlyCamera->eye,	// Eye
								   ::g_pFlyCamera->getAtInWorldSpace(),		// At
								   ::g_pFlyCamera->getUpVector() );// Up
			break;
		case eCameraTypes::FOLLOW_CAMERA:
			glUniform3f( eyeLocation_location, ::g_pFollowCamera->eye.x, ::g_pFollowCamera->eye.y, ::g_pFollowCamera->eye.z );
			matView = glm::lookAt( ::g_pFollowCamera->eye,	// Eye
								   ::g_pFollowCamera->target,		// At
								   ::g_pFollowCamera->up );// Up
			break;
		}


		GLint bIsLightVolumne_UniLoc = glGetUniformLocation( program, "bIsLightVolume" );
		glUniform1f( bIsLightVolumne_UniLoc, (float)GL_FALSE );


		glUniformMatrix4fv( matView_location, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv( matProj_location, 1, GL_FALSE, glm::value_ptr(matProjection));


//		pTheOneLight->SetRelativeDirectionByLookAt( ::g_pRogerRabbit->position );

			
		// Do all this ONCE per frame
		glUniform4f( pTheOneLight->position_UniLoc, 
						pTheOneLight->position.x, pTheOneLight->position.y, pTheOneLight->position.z, 1.0f );
		glUniform4f( pTheOneLight->diffuse_UniLoc, 
						pTheOneLight->diffuse.x, pTheOneLight->diffuse.y, pTheOneLight->diffuse.z, 1.0f );
		glUniform4f( pTheOneLight->param2_UniLoc, 1.0f, 0.0f, 0.0f, 0.0f );	// Turns it "on")
		glUniform4f( pTheOneLight->atten_UniLoc, 
						pTheOneLight->atten.x, pTheOneLight->atten.y, pTheOneLight->atten.z, pTheOneLight->atten.w );

		// Now pass the things we need for spots and directional, too:
		glUniform4f( pTheOneLight->direction_UniLoc, 
						pTheOneLight->direction.x, 
						pTheOneLight->direction.y,
						pTheOneLight->direction.z,
						pTheOneLight->direction.w );
		glUniform4f( pTheOneLight->param1_UniLoc, 
						pTheOneLight->param1.x,		// lightType
						pTheOneLight->param1.y,		// inner angle
						pTheOneLight->param1.z,		// outer angle
						pTheOneLight->param1.w );	// TBD


		cMeshSceneObject* pDebugSphere = findObjectByFriendlyName("DebugSphere");
		pDebugSphere->bIsVisible = true;
		pDebugSphere->bDontLight = true;
//			glm::vec3 oldColour = pDebugSphere->objColour;
		glm::vec4 oldDiffuse = pDebugSphere->materialDiffuse;
		float oldScale = pDebugSphere->uniformScale;
//			pDebugSphere->objColour = glm::vec3(255.0f/255.0f, 105.0f/255.0f, 180.0f/255.0f);
		pDebugSphere->setDiffuseColour( glm::vec3(255.0f/255.0f, 105.0f/255.0f, 180.0f/255.0f) );
		pDebugSphere->bUseVertexColour = false;
		pDebugSphere->position = glm::vec3(pTheOneLight->position);
		glm::mat4 matBall(1.0f);

		// For several reasons, the light isn't 'lit up',
		//	but to make it more visible, there's 3 'nested' 
		//	spheres. Look slightly to the right of the bunnies.
//			pDebugSphere->objColour = glm::vec3(1.0f,1.0f,1.0f);			
		pDebugSphere->materialDiffuse = oldDiffuse;
		pDebugSphere->uniformScale = 0.5f;			// Position
		DrawObject( pDebugSphere, matBall, program );

		if ( ::g_bDrawDebugLightSpheres )
		{


			const float ACCURACY_OF_DISTANCE = 0.0001f;
			const float INFINITE_DISTANCE = 10000.0f;

			float distance90Percent = 
				pLightHelper->calcApproxDistFromAtten( 0.90f, ACCURACY_OF_DISTANCE,
													INFINITE_DISTANCE,
												   pTheOneLight->atten.x, 
												   pTheOneLight->atten.y, 
												   pTheOneLight->atten.z );

			pDebugSphere->uniformScale = distance90Percent;			// 90% brightness
			//pDebugSphere->objColour = glm::vec3(1.0f,1.0f,0.0f);
			pDebugSphere->setDiffuseColour( glm::vec3(1.0f,1.0f,0.0f) );
			DrawObject( pDebugSphere, matBall, program );

//			pDebugSphere->objColour = glm::vec3(0.0f,1.0f,0.0f);	// 50% brightness
			pDebugSphere->setDiffuseColour( glm::vec3(0.0f,1.0f,0.0f) );
			float distance50Percent = 
				pLightHelper->calcApproxDistFromAtten( 0.50f, ACCURACY_OF_DISTANCE,
													INFINITE_DISTANCE,
												   pTheOneLight->atten.x, 
												   pTheOneLight->atten.y, 
												   pTheOneLight->atten.z );
			pDebugSphere->uniformScale = distance50Percent;
			DrawObject( pDebugSphere, matBall, program );

//			pDebugSphere->objColour = glm::vec3(1.0f,0.0f,0.0f);	// 25% brightness
			pDebugSphere->setDiffuseColour( glm::vec3(1.0f,0.0f,0.0f) );
			float distance25Percent = 
				pLightHelper->calcApproxDistFromAtten( 0.25f, ACCURACY_OF_DISTANCE,
													INFINITE_DISTANCE,
												   pTheOneLight->atten.x, 
												   pTheOneLight->atten.y, 
												   pTheOneLight->atten.z );
			pDebugSphere->uniformScale = distance25Percent;
			DrawObject( pDebugSphere, matBall, program );

			float distance1Percent = 
				pLightHelper->calcApproxDistFromAtten( 0.01f, ACCURACY_OF_DISTANCE,
													INFINITE_DISTANCE,
												   pTheOneLight->atten.x, 
												   pTheOneLight->atten.y, 
												   pTheOneLight->atten.z );
//			pDebugSphere->objColour = glm::vec3(0.0f,0.0f,1.0f);	// 1% brightness
			//pDebugSphere->setDiffuseColour( glm::vec3(0.0f,0.0f,1.0f) );
			pDebugSphere->uniformScale = distance1Percent; 
			DrawObject( pDebugSphere, matBall, program );

//			pDebugSphere->objColour = oldColour;
			pDebugSphere->materialDiffuse = oldDiffuse;
			pDebugSphere->uniformScale = oldScale;
			pDebugSphere->bIsVisible = false;
		}//if(::g_bDrawDebugLightSpheres)

		// Call the debug renderer call
//#ifdef _DEBUG
//		::g_pDebugRendererACTUAL->RenderDebugObjects( matView, matProjection, deltaTime );
//#endif 


		{
			// ***************************************
			 // Draw the skybox first 
			cMeshSceneObject* pSkyBox = findObjectByFriendlyName("SkyBoxObject");
//			cMeshObject* pSkyBox = findObjectByFriendlyName("SkyPirate");
			// Place skybox object at camera location
			pSkyBox->position = g_pFlyCamera->eye;
			pSkyBox->bIsVisible = true;
			pSkyBox->bIsWireFrame = false;


			// Bind the cube map texture to the cube map in the shader
			GLuint cityTextureUNIT_ID = 30;			// Texture unit go from 0 to 79
			glActiveTexture( cityTextureUNIT_ID + GL_TEXTURE0 );	// GL_TEXTURE0 = 33984

			int cubeMapTextureID = ::g_pTheTextureManager->getTextureIDFromName("CityCubeMap");

			// Cube map is now bound to texture unit 30
			//glBindTexture( GL_TEXTURE_2D, cubeMapTextureID );
			glBindTexture( GL_TEXTURE_CUBE_MAP, cubeMapTextureID );

			//uniform samplerCube textureSkyBox;
			GLint skyBoxCubeMap_UniLoc = glGetUniformLocation( program, "textureSkyBox" );
			glUniform1i( skyBoxCubeMap_UniLoc, cityTextureUNIT_ID );

			//uniform bool useSkyBoxTexture;
			GLint useSkyBoxTexture_UniLoc = glGetUniformLocation( program, "useSkyBoxTexture" );
			glUniform1f( useSkyBoxTexture_UniLoc, (float)GL_TRUE );

			glm::mat4 matIdentity = glm::mat4(1.0f);
			DrawObject( pSkyBox, matIdentity, program );

			//glEnable( GL_CULL_FACE );
			//glCullFace( GL_BACK );

			pSkyBox->bIsVisible = false;
			glUniform1f( useSkyBoxTexture_UniLoc, (float)GL_FALSE );

			// ***************************************
		}

		//*************************************************
		if (mazeProcessingThreads)
		{
			for (int i = 0; i < dalekMax; i++)
			{
				//Wait for thread
				if (WaitForSingleObject((hThreadHandle[i]), INFINITE))
				{
					CloseHandle(hThreadHandle[i]);
				}
			}
		}
		else
		{
			for (int i = 0; i < dalekMax; i++)
			{
				std::string dalekName = "Dalek" + std::to_string(i);

				cMeshSceneObject* pDalek = findObjectByFriendlyName(dalekName);

				StartPath(pDalek);
			}
		}

		//Apply and reset position changes
		/*for (int i = 0; i < positionChanges.size(); i++)
		{
			std::string dalekName = "Dalek" + std::to_string(i);

			cMeshSceneObject* pDalek = findObjectByFriendlyName(dalekName);

			glm::vec3 change = positionChanges[dalekName];

			pDalek->position += change;

			positionChanges.erase(dalekName);
		}*/

		EnterCriticalSection(&CS_Dalek_Pos);
		DrawScene_Simple( vec_pObjectsToDraw, program, 0 );
		LeaveCriticalSection(&CS_Dalek_Pos);


		// ****************************************
		// Now the entire scene has been drawn 
		// ****************************************

		// *****************************************
		// 2nd pass
		// *****************************************

		// 1. Set the Framebuffer output to the main framebuffer
//		glBindFramebuffer( GL_FRAMEBUFFER, 0 );		// Points to the "regular" frame buffer

		// Point the frame buffer output to the "effects" pass FBO
		glBindFramebuffer( GL_FRAMEBUFFER, ::g_pFBOEffectsPass->ID );
		::g_pFBOEffectsPass->clearBuffers();
													// Get the size of the actual (screen) frame buffer
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

		glEnable( GL_DEPTH_TEST );	// When drawing, checked the existing depth
		glEnable( GL_CULL_FACE );	// Discared "back facing" triangles

		// 2. Clear everything **ON THE MAIN FRAME BUFFER** 
		//     (NOT the offscreen buffer)
 		// This clears the ACTUAL screen framebuffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// 3. Bind 1 texture (what we drew)
		cMeshSceneObject* p2SidedQuad = findObjectByFriendlyName("2SidedQuad");
		p2SidedQuad->bIsVisible = true;
		p2SidedQuad->b_HACK_UsesOffscreenFBO = true;
		p2SidedQuad->bDontLight = true;
		p2SidedQuad->bUseVertexColour = false;
		//p2SidedQuad->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		p2SidedQuad->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//p2SidedQuad->bIsWireFrame = true;
		// Rotate it so it's "up and down"
		p2SidedQuad->setMeshOrientationEulerAngles( 90.0f, 0.0f, 90.0f, true );
		p2SidedQuad->position.z = 1.0f;
//		p2SidedQuad->setUniformScale( 1.0f );
		// Set the scale of the full screen quad to be the same as the FBO 
		//	aspect ratio we used

		ratio = 3840.0f / (float) 2160.0f;

		p2SidedQuad->uniformScale = ratio;
		//p2SidedQuad->nonUniformScale.x = 1.0f;
		//p2SidedQuad->nonUniformScale.y = 1.0f;
		//p2SidedQuad->nonUniformScale.z = ratio;

		// Tell the shader this is the 2nd pass...
		// This will run a very simple shader, which
		//  does NO lighting, and only samples from a single texture
		//  (for now: soon there will be multiple textures)
		glUniform1f( renderPassNumber_UniLoc, 2.0f );	// Tell shader it's the 2nd pass

		::pTheShaderManager->SetUniformVar1f( program, "screenWidth", (float)width );
		::pTheShaderManager->SetUniformVar1f( program, "screenHeight", (float)height );

		// Set the view transform so that the camera movement isn't impacted 


		glm::vec3 cameraFullScreenQuad = glm::vec3( 0.0, 0.0, 0.0f );

		glUniform3f( eyeLocation_location, 0.0f, 0.0f, 0.0f );
		matView = glm::lookAt( glm::vec3(0.0f, 0.0f, +2.0f),			// Eye
								cameraFullScreenQuad,					// At
								glm::vec3(0.0f, 1.0f, 0.0f) );			// Up

		glUniformMatrix4fv( matView_location, 1, GL_FALSE, glm::value_ptr(matView));

		// 4. Draw a single quad		
		glm::mat4 matModel = glm::mat4(1.0f);	// identity
		DrawObject( p2SidedQuad, matModel, program, 2 );
		// Make this invisible for the "regular" pass
		p2SidedQuad->bIsVisible = false;


		// *****************************************
		// 3rd pass
		// *****************************************

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );		// Points to the "regular" frame buffer

		// 2. Clear everything **ON THE MAIN FRAME BUFFER** 
		//     (NOT the offscreen buffer)
 		// This clears the ACTUAL screen framebuffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUniform1f( renderPassNumber_UniLoc, 3.0f );	// Tell shader it's the 2nd pass

		p2SidedQuad->bIsVisible = true;
														// 4. Draw a single quad		
		matModel = glm::mat4(1.0f);	// identity
		DrawObject( p2SidedQuad, matModel, program, 3 );
		// Make this invisible for the "regular" pass
		p2SidedQuad->bIsVisible = false;



//		// Light volume sphere, drawn at location of actual light.
//		// It's sized to be about the size of the "throw" of the original light
//		cMeshSceneObject* pLightSphere = findObjectByFriendlyName("DebugSphere");
//		bool bOldIsWireFrame = pLightSphere->bIsWireFrame;
//		bool bOldIsVisible = pLightSphere->bIsVisible;
//		oldScale = pLightSphere->uniformScale;
//		glm::vec3 oldPosition = pLightSphere->position;
//
//		// Calculate the approximate size of the light volume
//		const float ACCURACY_OF_DISTANCE = 0.0001f;
//		const float INFINITE_DISTANCE = 10000.0f;
//
//		float sizeOfLightVolume = 
//			pLightHelper->calcApproxDistFromAtten( 0.01f, ACCURACY_OF_DISTANCE,
//												INFINITE_DISTANCE,
//												pTheOneLight->atten.x, 
//												pTheOneLight->atten.y, 
//												pTheOneLight->atten.z );
//		pLightSphere->uniformScale = sizeOfLightVolume;
////		pLightSphere->uniformScale = 10.0f;
//		pLightSphere->bIsWireFrame = true;
//		pLightSphere->bIsVisible = true;
////		pLightSphere->bIsALightVolume = true;
//		pLightSphere->position = pTheOneLight->position;
////		pLightSphere->position = glm::vec3(0.0f,0.0f,0.0f);
////		pLightSphere->bUseVertexColour = true;
//		pLightSphere->materialDiffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
//	
//
//		glUniform1f( bIsLightVolumne_UniLoc, (float)GL_TRUE );
//
//
//		glm::mat4 matModel = glm::mat4(1.0f);	// identity
//		DrawObject( pLightSphere, matModel, program );
//
//		pLightSphere->bIsWireFrame = bOldIsWireFrame;
//		pLightSphere->bIsVisible = bOldIsVisible;
//		pLightSphere->uniformScale = oldScale;
//		pLightSphere->position = oldPosition;
//		pLightSphere->bIsALightVolume = false;



		//pBunny->nonUniformScale = BunnyOldScale;
		//pBunny->position = BunnyOldPos;


		//********************************************************

		// At this point the scene is drawn...
		UpdateWindowTitle(window);

		glfwSwapBuffers(window);		// Shows what we drew

        glfwPollEvents();

		ProcessAsyncKeys(window);

		ProcessAsyncMouse(window);

		ProcessAsyncJoysticks(window, ::g_pJoysticks);


		//************************************************
		if (mazeProcessingThreads)
		{
			for (int i = 0; i < dalekMax; i++)
			{
				std::string dalekName = "Dalek" + std::to_string(i);

				cMeshSceneObject* pDalek = findObjectByFriendlyName(dalekName);

				PVOID* pDalekV = (PVOID*)pDalek;

				//Start the thread
				hThreadHandle[i] = CreateThread(NULL,
					0,
					//&MoveDalek, //the thread's function
					&FindPathToPlayer, //the thread's function
					pDalekV, //value being pased
					0,
					(DWORD*)(&(phThread[i])));
			}
		}


    }//while (!glfwWindowShouldClose(window))

	ShutDown();


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


void ShutDown(void)
{
	// Delete stuff
	delete pTheShaderManager;
	delete ::g_pTheVAOMeshManager;
	delete ::g_pTheTextureManager;
	delete ::g_pDebugRenderer;
	delete ::g_pFlyCamera;
	delete ::g_pJoysticks;

	// TODO: Other things to delete, too


	// Get rid of any particle emitters
	
	for ( std::map<std::string /*name*/, cParticleEmitter* >::iterator itPE = ::g_map_pParticleEmitters.begin();
		  itPE != ::g_map_pParticleEmitters.end(); itPE++ )
	{
		cParticleEmitter* pThePE = itPE->second;

		delete pThePE;
	}
	::g_map_pParticleEmitters.clear();



	return;
}




void UpdateWindowTitle(GLFWwindow* window)
{
	// #include sstream 
	std::stringstream ssTitle;

	ssTitle			// std::cout 
		<< pTheOneLight->atten.x << ", " 
		<< pTheOneLight->atten.y << ", " 
		<< pTheOneLight->atten.z;

	glfwSetWindowTitle( window, ssTitle.str().c_str() );

	return;
}

cMeshSceneObject* findObjectByFriendlyName(std::string theNameToFind)
{
	for ( unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++ )
	{
		// Is this it? 500K - 1M
		// CPU limited Memory delay = 0
		// CPU over powered (x100 x1000) Memory is REAAAAALLY SLOW
		if ( vec_pObjectsToDraw[index]->friendlyName == theNameToFind )
		{
			return vec_pObjectsToDraw[index];
		}
	}

	// Didn't find it.
	return NULL;	// 0 or nullptr
}


cMeshSceneObject* findObjectByUniqueID(unsigned int ID_to_find)
{
	for ( unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++ )
	{
		if ( vec_pObjectsToDraw[index]->getUniqueID() == ID_to_find )
		{
			return vec_pObjectsToDraw[index];
		}
	}

	// Didn't find it.
	return NULL;	// 0 or nullptr
}


// For physics checkpoint #4
// This is global because I need to update this with the keyboard
float g_ProjectileInitialVelocity = 10.0f;

void DoPhysicsCheckpointNumberFour(double deltaTime)
{
	// Draw a target right in front of the camera, like where it's pointing "At"...
	// (get it?? "AT" POinting AT!????)

	// THIS at is "just in front" of the camera.
	// It's exactly 1 unit away
	glm::vec3 cameraAt = ::g_pFlyCamera->getAtInWorldSpace();

	cMeshSceneObject* pDebugBall = ::findObjectByFriendlyName("DebugSphere");

//	//Draw a red sphere at the "at" position of the camera ("right" in front of the camera)
//	pDebugBall->position = cameraAt;
//	pDebugBall->setDiffuseColour( glm::vec3(1.0f, 0.0f, 0.0f) );
//	pDebugBall->bIsVisible = true;
//	pDebugBall->setUniformScale( 0.1f );
//	
//	glm::mat4 matWorld = glm::mat4(1.0f);
//	DrawObject( pDebugBall, matWorld, ::pTheShaderManager->getIDFromFriendlyName("BasicUberShader") );
//	
//	pDebugBall->bIsVisible = false;


	// To draw a "trail" of spheres, we run the physics integration step
	// a number of times, based on the projectile velocity

	// Calculate the 3D "directed" velocity, assuming the projectile velocity is 
	//	"forward" from the camera...
	// 1. Get the camera "at" (direction of the camera)
	glm::vec3 projVelWorldSpace = ::g_pFlyCamera->getCameraDirection() * ::g_ProjectileInitialVelocity;
	glm::vec3 projPosition = ::g_pFlyCamera->eye;
	glm::vec3 projAccel = g_ACCEL_GRAVITY;


//	projVelWorldSpace = glm::vec3( 0.0f, 0.0f, 10.0f );


	// Run a number of steps of the simulation, drawing a sphere at X intervals of time

	float timeStep = 0.25f;
	float howLongWeGonnaRun = 10.0f;

	for ( float time = 0.0; time < howLongWeGonnaRun; time += timeStep )
	{
		// Stolen from the PhysicsUpdate()
		// Gravity is ???

		// Update velocity from acceleration...
		projVelWorldSpace.x = projVelWorldSpace.x + ( projAccel.x * timeStep );
		projVelWorldSpace.y = projVelWorldSpace.y + ( projAccel.y * timeStep );
		projVelWorldSpace.z = projVelWorldSpace.z + ( projAccel.z * timeStep );

		// Update position from velocity
		projPosition.x = projPosition.x + ( projVelWorldSpace.x * timeStep );
		projPosition.y = projPosition.y + ( projVelWorldSpace.y * timeStep );
		projPosition.z = projPosition.z + ( projVelWorldSpace.z * timeStep );

		// Draw a sphere at each of these locations...
		pDebugBall->position = projPosition;
		pDebugBall->setDiffuseColour( glm::vec3(0.8f, 0.8f, 0.8f) );
		pDebugBall->bIsVisible = true;
		pDebugBall->uniformScale = 0.075f;

		glm::mat4 matWorld = glm::mat4(1.0f);
		DrawObject( pDebugBall, matWorld, ::pTheShaderManager->GetShaderIDFromName("BasicUberShader") );

		pDebugBall->bIsVisible = false;

	}// for ( double time = 0.0;...


	return;
}


void InitSound(void)
{
	::g_pSound = new cSound();


	std::string error;
	if ( !::g_pSound->Init(error) )
	{
		std::cout << "Sound.Init() returned false because: " << error << std::endl;
		std::cout << "Perhaps you need OpenAL installed?" << std::endl;
		std::cout << "You can get it here: https://openal.org/downloads/" << std::endl;
		std::cout << " (not that I'm telling you what to do or anything, but you won't hear " << std::endl;
		std::cout << "  any sweet mechanical sounds until you download and instal OpenAL.)" << std::endl;
	}
	
	std::cout << "You seem to have OpenAL (sound) on your system. This is good." << std::endl;

	if ( ::g_pSound->IsGoodToGo() )
	{
		::g_pSound->setFileBasePath( "assets//sounds//" );
		if ( ! ::g_pSound->LoadFromFile( "myNoise_NORTHERN_edited.wav", "Happy", cSound::sSourceType::TYPE_STREAMING ) )
		{
			std::cout << "Warning: didn't load the myNoise_NORTHERN_edited.wav file." << std::endl;
		}
		else
		{
			std::cout << "Sad sounds are loaded OK" << std::endl;
		}

		if ( ! ::g_pSound->LoadFromFile( "myNoise_THUNDER_edited.wav", "Sad", cSound::sSourceType::TYPE_STREAMING ) )
		{
			std::cout << "Warning: didn't load the myNoise_THUNDER_edited.wav file." << std::endl;
		}
		else
		{	
			std::cout << "Happy sounds are loaded OK" << std::endl;
		}
	}//if ( ::g_pSound->IsGoodToGo() )




	return;
}


void CreateDaleks(void)
{
	::g_pDalekMangerBuffer = new cDalekManagerTripleBuffer();


	cRandThreaded* pRT = new cRandThreaded();


	::g_pDalekMangerBuffer->InitBuffers( ::g_NUMBEROFDALEKS );

	for ( unsigned int count = 0; 
		  count != ::g_NUMBEROFDALEKS; count++ )
	{
		cDalek* pCurDalek = new cDalek();

		pCurDalek->bIsAlive = true;

		pCurDalek->CreateThreadAndStartUpdating( count, 
												 ::g_pDalekMangerBuffer, pRT );

		::g_vec_pDaleks.push_back( pCurDalek );
	}
		
	return;
}




//void LoadTerrainAABB(void)
//{
//	// *******
//	// This REALLY should be inside the cAABBHierarchy, likely... 
//	// *******
//
//
//	// Read the graphics mesh object, and load the triangle info
//	//	into the AABB thing.
//	// Where is the mesh (do the triangles need to be transformed)??
//	
//	cMeshSceneObject* pTerrain = findObjectByFriendlyName("The Terrain");
//
//	sModelDrawInfo terrainMeshInfo;
//	terrainMeshInfo.meshFileName = pTerrain->meshName;
//
//	::g_pTheVAOMeshManager->FindDrawInfoByModelName( terrainMeshInfo );
//
//
//	// How big is our AABBs? Side length?
//	float sideLength = 20.0f;		// Play with this lenght
//									// Smaller --> more AABBs, fewer triangles per AABB
//									// Larger --> More triangles per AABB
//
//	for ( unsigned int triIndex = 0; triIndex != terrainMeshInfo.numberOfTriangles; triIndex++ )
//	{
//		// for each triangle, for each vertex, determine which AABB the triangle should be in
//		// (if your mesh has been transformed, then you need to transform the tirangles 
//		//  BEFORE you do this... or just keep the terrain UNTRANSFORMED)
//
//		sPlyTriangle currentTri = terrainMeshInfo.pTriangles[triIndex];
//
//		sPlyVertex currentVerts[3];	
//		currentVerts[0] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_1];
//		currentVerts[1] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_2];
//		currentVerts[2] = terrainMeshInfo.pVerticesFromFile[currentTri.vertex_index_3];
//
//		// This is the structure we are eventually going to store in the AABB map...
//		cAABB::sAABB_Triangle curAABBTri;
//		curAABBTri.verts[0].x = currentVerts[0].x;
//		curAABBTri.verts[0].y = currentVerts[0].y;
//		curAABBTri.verts[0].z = currentVerts[0].z;
//		curAABBTri.verts[1].x = currentVerts[1].x;
//		curAABBTri.verts[1].y = currentVerts[1].y;
//		curAABBTri.verts[1].z = currentVerts[1].z;
//		curAABBTri.verts[2].x = currentVerts[2].x;
//		curAABBTri.verts[2].y = currentVerts[2].y;
//		curAABBTri.verts[2].z = currentVerts[2].z;
//
//		// Is the triangle "too big", and if so, split it (take centre and make 3 more)
//		// (Pro Tip: "too big" is the SMALLEST side is greater than HALF the AABB length)
//		// Use THOSE triangles as the test (and recursively do this if needed),
//		// +++BUT+++ store the ORIGINAL triangle info NOT the subdivided one
//		// 
//		// For the student to complete... 
//		// 
//
//
//		for ( unsigned int vertIndex = 0; vertIndex != 3; vertIndex++ )
//		{
//			// What AABB is "this" vertex in? 
//			unsigned long long AABB_ID = 
//				cAABB::generateID( curAABBTri.verts[0],
//								   sideLength );
//
//			// Do we have this AABB alredy? 
//			std::map< unsigned long long/*ID AABB*/, cAABB* >::iterator itAABB
//				= ::g_pTheTerrain->m_mapAABBs.find( AABB_ID );
//
//			if( itAABB == ::g_pTheTerrain->m_mapAABBs.end() )
//			{
//				// We DON'T have an AABB, yet
//				cAABB* pAABB = new cAABB();
//				// Determine the AABB location for this point
//				// (like the generateID() method...)
//				glm::vec3 minXYZ = curAABBTri.verts[0];
//				minXYZ.x = (floor(minXYZ.x / sideLength)) * sideLength;
//				minXYZ.y = (floor(minXYZ.y / sideLength)) * sideLength;
//				minXYZ.z = (floor(minXYZ.z / sideLength)) * sideLength;
//
//				pAABB->setMinXYZ( minXYZ );
//				pAABB->setSideLegth( sideLength );
//				// Note: this is the SAME as the AABB_ID...
//				unsigned long long the_AABB_ID = pAABB->getID();
//
//				::g_pTheTerrain->m_mapAABBs[the_AABB_ID] = pAABB;
//
//				// Then set the iterator to the AABB, by running find again
//				itAABB = ::g_pTheTerrain->m_mapAABBs.find( the_AABB_ID );
//			}//if( itAABB == ::g_pTheTerrain->m_mapAABBs.end() )
//
//			// At this point, the itAABB ++IS++ pointing to an AABB
//			// (either there WAS one already, or I just created on)
//
//			itAABB->second->vecTriangles.push_back( curAABBTri );
//
//		}//for ( unsigned int vertIndex = 0;
//
//	}//for ( unsigned int triIndex
//
//
//
//	// At runtime, need a "get the triangles" method...
//
//	return;
//}
//

