#ifndef _cGameEntity_HG_
#define _cGameEntity_HG_

// This represents something that's in the scene. 
// This usually ties to a mesh, but doesn't need to be.

#include <string>
#include <vector>

// Forward declares
class cMeshSceneObject;
class cSimpleAssimpSkinnedMesh;
class cSoundDesc;

// For position and physics (of 'root' entity
class cPhysicalProperties;
#include "Physics/iPhysicalObject.h"

#include "cNameValuePair.h"

class cGameEntity : public iPhysicalObject
{
public:
	cGameEntity();
	~cGameEntity();

	std::string friendlyName; 

	unsigned int getUniqueID(void);

	// Here's the information about this entity
	// To simplify, we'll assume that each game entity has only one "root" scene object
	//	or one skinned mesh object
	// (the mesh scene objects can have child objects)
	cMeshSceneObject*			m_pMeshSceneObject;
	cSimpleAssimpSkinnedMesh*	m_pSkinnedMesh;

	std::vector<cSoundDesc*>	m_vec_pSounds;

	// NULL if not something that has a location
	cPhysicalProperties* m_pPhysProps;	
	// STARTOF: From iPhysicalObject interface
	virtual void SetPhysState( cPhysicalProperties &PhysState );
	// Returns false if there isn't any physical state on this object
	virtual bool GetPhysState( cPhysicalProperties &PhysState );
	virtual bool HasPhysState(void);
	// ENDOF: From iPhysicalObject interface


private:
	unsigned int m_uniqueID;
	static unsigned int m_nextID;
	static const unsigned int STARTING_ID_NUMBER = 100;

};

#endif 
