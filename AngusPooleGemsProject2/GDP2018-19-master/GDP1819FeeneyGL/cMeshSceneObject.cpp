#include "cMeshSceneObject.h"

cMeshSceneObject::cMeshSceneObject() 
{
	this->position = glm::vec3(0.0f);
//	this->nonUniformScale = glm::vec3(1.0f);
	this->uniformScale = 1.0f;

//	this->preRotation = glm::vec3(0.0f);
//	this->postRotation = glm::vec3(0.0f);
	this->setMeshOrientationEulerAngles( glm::vec3(0.0f,0.0f,0.0f));

//	this->m_meshQOrientation = glm::quat( glm::vec3(0.0f,0.0f,0.0f));

	// 
	this->bIsVisible = true;
	this->bIsWireFrame = false;

	this->bIsALightVolume = false;

	// HACK: See class definition for why this is a hack
	this->b_HACK_UsesOffscreenFBO = false;

	// Set unique ID
	this->m_uniqueID = cMeshSceneObject::m_NextID;
	// Increment
	cMeshSceneObject::m_NextID++;	// 32 bit - 4 billion

//	this->objColour = glm::vec3(1.0f, 1.0f, 1.0f);	// white by default

	this->materialDiffuse = glm::vec4(1.0f,1.0f,1.0f,1.0f);	
	this->materialSpecular = glm::vec4(1.0f,1.0f,1.0f,1.0f);

	this->bUseVertexColour = false;

	this->bDontLight = false;

	this->velocity = glm::vec3(0.0f);
	this->accel = glm::vec3(0.0f);
	this->bIsUpdatedByPhysics = false;	// physics ignores by default

//	this->radius = 0.0f;

	this->pDebugRenderer = NULL;

	// Set shape to NULL
	this->pTheShape = NULL;
	this->shapeType = cMeshSceneObject::UNKOWN_SHAPE;

	// Skinned mesh and animations:
	this->pSimpleSkinnedMesh = NULL;
	this->pAniState = NULL;	

	this->travelling = false;
	this->distanceTraveled = 0;
	this->targetDistance = 0;
	this->aiPath = 0;

	return;
}

cMeshSceneObject::~cMeshSceneObject()
{
	//TODO: Delete stuff

	return;
}

void cMeshSceneObject::setDiffuseColour(glm::vec3 newDiffuse)
{
	this->materialDiffuse = glm::vec4( newDiffuse, this->materialDiffuse.a );
	return;
}

void cMeshSceneObject::setAlphaTransparency(float newAlpha)
{
	// RGB A
	this->materialDiffuse.a = newAlpha;	
	return;
}

void cMeshSceneObject::setSpecularColour(glm::vec3 colourRGB)
{
	this->materialSpecular.r = colourRGB.r;
	this->materialSpecular.g = colourRGB.g;
	this->materialSpecular.b = colourRGB.b;
	return;
}

void cMeshSceneObject::setSpecularPower(float specPower)
{
	this->materialSpecular.a = specPower;
	return;
}



	
//static unsigned int m_NextID; //= 0;

//static 
unsigned int cMeshSceneObject::m_NextID = cMeshSceneObject::STARTING_ID_VALUE;


void cMeshSceneObject::Update(double deltaTime)
{
	if ( this->bIsUpdatedByPhysics )
	{
		// Figure out a line showing the velocity
		glm::vec3 vVel = this->velocity + this->position;
		glm::vec3 vAcc = this->accel + this->position;

		if ( this->pDebugRenderer )		// != NULL
		{
//			this->pDebugRenderer->addLine( this->position, vVel, 
//										   glm::vec3( 0.0f, 1.0f, 0.0f ), 0.0f );
//
//			this->pDebugRenderer->addLine( this->position, vAcc, 
//										   glm::vec3( 0.0f, 1.0f, 1.0f ), 0.0f );
		}//if ( this->pDebugRenderer )	
	}//if ( this->bIsUpdatedByPhysics )

	return;
}
