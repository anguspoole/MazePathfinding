#ifndef _cMeshSceneObject_HG_
#define _cMeshSceneObject_HG_


#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL		// To get glm quaternion stuff to compile
#include <glm/gtx/quaternion.hpp>	// Note strange folder
#include <string>
#include <vector>

// Add the ability to add debug shapes to the debug renderer
// **NOTE** it's the INTERFACE that I have, NOT the class
#include "DebugRenderer/iDebugRenderer.h"


// This is for the skinned mesh and animation
class cSimpleAssimpSkinnedMesh;	// Forward declare
class cAnimationState;			// Forward declare 


struct sSphere
{
	sSphere( float theRadius )
	{
		this->radius = theRadius;
	}
	float radius;
};
struct sTriangle
{
	glm::vec3 v[3];
};
struct sAABB
{
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
};

struct sAOB  
{
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	//glm::vec3 orientationEuler;
	glm::mat3 matOrientation;
};

struct sTextureInfo 
{ 
      std::string name; 
      int cachedTextureID; 	// Set to -1 by default
      float strength;		// Set to 0.0f by default
	  // Added: specific FBOs
};

struct sLODInfo
{
	sLODInfo() : 
		minDistance(FLT_MAX) {}
	std::string meshName;	// Model to draw
	sLODInfo(std::string meshName_) 
	{
		this->meshName = meshName_;
		this->minDistance = FLT_MAX;
	};
	sLODInfo(std::string meshName_, float minDistanceToDraw ) 
	{
		this->meshName = meshName_;
		this->minDistance = minDistanceToDraw;
	};

	// If it's at or closer than this, draw this model
	float minDistance;
};


class cMeshSceneObject
{
public:
	cMeshSceneObject();
	~cMeshSceneObject();


//	glm::vec3 preRotation;		// Happens BEFORE translation (move)
////	glm::vec3 position;
//	glm::vec3 postRotation;		// Happens AFTER translation (move)
private:
	glm::quat m_meshQOrientation;		// Like a mat3x3 rotation matrix
public:
	glm::quat getQOrientation(void)	{	return this->m_meshQOrientation;	};
	void setQOrientation( glm::quat newOrientation )
	{
		this->m_meshQOrientation = newOrientation;
	}
	void setMeshOrientationEulerAngles( glm::vec3 newAnglesEuler, bool bIsDegrees = false );
	void setMeshOrientationEulerAngles( float x, float y, float z, bool bIsDegrees = false );
	void adjMeshOrientationEulerAngles( glm::vec3 adjAngleEuler, bool bIsDegrees = false );
	void adjMeshOrientationEulerAngles( float x, float y, float z, bool bIsDegrees = false );
	void adjMeshOrientationQ( glm::quat adjOrientQ );


//	void setUniformScale(float scale);
//	glm::vec3 nonUniformScale;
	float uniformScale;

	// Things we can use to find this object 
	std::string friendlyName;		// Human "Bob" "Ali" "Player"

//	std::string meshName;	// Model to draw

	// If there's only 1, then we AREN'T using LOD
	std::vector<sLODInfo> vecLODMeshs;	

	bool bIsWireFrame;	//  = true;  C++
	bool bIsVisible;

	//HACK: This will be replaced with something more sophisticated, later
	// For now, there is only one off screen FBO
	bool b_HACK_UsesOffscreenFBO;
	unsigned int renderPassNumber;

	// ignore this for now...
	// Around its own axis (NOT the origin)
	//glm::vec3 orientation;

//	glm::vec3 objColour;
	glm::vec4 materialDiffuse;		// RGB+Alpha
	void setDiffuseColour(glm::vec3 newDiffuse);
	void setAlphaTransparency(float newAlpha);
	glm::vec4 materialSpecular;		// RGB+specular power
	void setSpecularColour(glm::vec3 colourRGB);
	void setSpecularPower(float specPower);

	bool bUseVertexColour;		// = false (default)

	bool bDontLight;		// If true, just object colour is used

	// If true, then will draw both front and back.
	bool bIsALightVolume;

	// Child objects inside this one
	std::vector< cMeshSceneObject* > vec_pChildObjectsToDraw;


	// Things that are updated by physics
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 accel;		// acceleration

	bool bIsUpdatedByPhysics;	// Phsyics "owns" the position	

	float mass;
	float inverseMass;

	// Assume that everything is a sphere...
	enum eShape
	{
		UNKOWN_SHAPE,
		SPHERE,
		TRIANGLE,
		AABB, AOB	// DON'T use these	
		// PLANE, AABB, etc., etc.
	};
	// Allows me to point to anything (any type)
	void* pTheShape;		// Details
	eShape shapeType;		// What shape it is

	//float radius;

	// 
	iDebugRenderer* pDebugRenderer;
	void Update(double deltaTime);

	// In theory, it's faster. 
	// Not likely. Suggestion, not a command
	inline unsigned int getUniqueID(void)
	{
		return this->m_uniqueID;
	}


	// Textures...
	std::vector<sTextureInfo> vecTextures;


	// Skinned mesh and animations:
	// If NULL, then object ISN'T a skinned mesh
	cSimpleAssimpSkinnedMesh*	pSimpleSkinnedMesh;	

	// HACK
	std::string currentAnimation;		
	cAnimationState*	pAniState;			

	// Extent Values for skinned mesh
	// These can be updated per frame, from the "update skinned mesh" call
	glm::vec3 minXYZ_from_SM_Bones;
	glm::vec3 maxXYZ_from_SM_Bones;
	// Store all the bones for this model, being updated per frame
	std::vector< glm::mat4x4 > vecObjectBoneTransformation;

	bool travelling;
	int distanceTraveled;
	int targetDistance;
	int aiPath;
	int mazeX;
	int mazeZ;
	int prevMazeX;
	int prevMazeZ;

private:
	unsigned int m_uniqueID;			// Number that's unique to this instance
	// static:
	// - There's only 1.
	// - All classes "see" it
	static unsigned int m_NextID; //= 0;	Can't assign as is static

	static const unsigned int STARTING_ID_VALUE = 1000;



};

#endif 
