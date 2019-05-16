#ifndef _globalStuff_HG_
#define _globalStuff_HG_


// globalStuff.h 

// These two OpenGL things are moved to the 
//	globalOpenGLStuff.h file
#include "globalOpenGLStuff.h"

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <map>	// For the particle emitters

#include "sPlyVertex.h"
#include "sPlyTriangle.h"
#include "cMeshSceneObject.h"
#include "sModelDrawInfo.h"
#include "cVAOMeshManager.h"

#include "cLightMangager.h"

#include "Error/CErrorLog.h"

#include "DebugRenderer/iDebugRenderer.h"

#include "cFlyCamera.h"
#include "cFollowCamera.h"

#include "cJoystickManager.h"

#include "TextureManager/cBasicTextureManager.h"

#include "../OpenALSound/cSound.h"

// The commands
#include "cCommandGroup.h"
// And the commands
#include "cFollowObjectCommand.h"
#include "cAnimationCurveCommand.h"
#include "cWait.h"

#include "cParticleEmitter.h"

// Skinned mesh
#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

// ... and more

//extern glm::vec3 g_CameraEye;	// = glm::vec3( 0.0, 0.0, +10.0f );
//extern glm::vec3 g_CameraAt;	// = glm::vec3( 0.0, 0.0, 0.0f );
extern cFlyCamera* g_pFlyCamera;
extern cFollowCamera* g_pFollowCamera;
enum eCameraTypes
{
	FLY_CAMERA,
	FOLLOW_CAMERA,
	UNKNOWN_CAMERA
};
extern eCameraTypes g_CameraInUse;

extern cJoystickManager* g_pJoysticks;

//// NOTE: These are JUST FOR NOW!!!
//extern glm::vec3 g_lightPos;	// = glm::vec3( 4.0f, 4.0f, 0.0f );
//extern float g_lightBrightness;	// = 1.0f;

extern cVAOMeshManager* g_pTheVAOMeshManager;

extern cBasicTextureManager* g_pTheTextureManager;

// 
// This is a global pointer to a mesh object. 
// You can get at object like this, or you can 
// search for them using the findObjectBy...() 
// functions.
extern cMeshSceneObject* g_pRogerRabbit;
// 

// Signature for the ply loader function
bool LoadPlyFileData( std::string fileName );

void LoadModelTypes( cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID );
void LoadModelTypes_ASYNC(cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID);
void LoadModelsIntoScene( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw );

// Skinned mesh model...
void LoadSkinnedMeshModel( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw, 
						   unsigned int shaderProgramID );

// TODO: Delete this
void LoadPlayerModels_LOJAM( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw );

cMeshSceneObject* findObjectByFriendlyName(std::string theNameToFind);
cMeshSceneObject* findObjectByUniqueID( unsigned int IDToFind );

void DrawObject( cMeshSceneObject* pCurrentMesh,
				 glm::mat4x4 &matModel, 
				 GLuint shaderProgramID, 
				 unsigned int RenderPassNumber = 0 );

// 
void DrawScene_Simple( std::vector<cMeshSceneObject*> vec_pMeshSceneObjects, 
					   GLuint shaderProgramID, 
					   unsigned int passNumber = 0 );


// *****************************************************************
// This is part of the physics stuff
// (really should be moved to the physics area, not here)
struct sClosestPointData
{
	glm::vec3 thePoint;
	unsigned int triangleIndex;
};
void CalculateClosestPointsOnMesh( sModelDrawInfo theMeshDrawInfo, 
								   glm::vec3 pointToTest, 
								   std::vector<sClosestPointData> &vecPoints );
// *****************************************************************


//https://stackoverflow.com/questions/9878965/rand-between-0-and-1
template <class T>
T getRandBetween0and1(void)
{
	return (T) ((double) rand() / (RAND_MAX)) ;
}

// Inspired by: https://stackoverflow.com/questions/686353/c-random-float-number-generation
template <class T>
T getRandInRange( T min, T max )
{
	double value = 
		min + static_cast <double> (rand()) 
		/ ( static_cast <double> (RAND_MAX/(static_cast<double>(max-min) )));
	return static_cast<T>(value);
}

extern iDebugRenderer* g_pDebugRenderer;

// If true, then the "ambient brightness" spheres will be drawn
extern bool g_bDrawDebugLightSpheres;	// = true


// For physics checkpoint #4
// This is global because I need to update this with the keyboard
extern float g_ProjectileInitialVelocity;

static const glm::vec3 g_ACCEL_GRAVITY = glm::vec3( 0.0f, -5.0f, 0.0f );

extern std::map<std::string /*name*/, cParticleEmitter* > g_map_pParticleEmitters;

extern cSound* g_pSound;// = NULL;



// Skinned mesh object 
//extern cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh; //= NULL;

// The main Frame Buffer Object (off screen rendering)
// (You can create one of these for each off screen rendered thing, if you'd like)
extern cFBO* g_pFBOMain;
// 2nd pass "effects" pass
extern cFBO* g_pFBOEffectsPass;

extern int mazeWidth;
extern int mazeHeight;
//extern cMazeMaker* maze;

#endif	// _globalStuff_HG_
