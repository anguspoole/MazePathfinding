#include "cGameEntity.h"

#include "Physics/cPhysicalProperties.h"

cGameEntity::cGameEntity()
{
	this->m_pMeshSceneObject = NULL;
	this->m_pSkinnedMesh = NULL;

	this->m_pPhysProps = NULL;

	this->m_uniqueID = cGameEntity::m_nextID++;
	return;
}

cGameEntity::~cGameEntity()
{

	return;
}

//static 
unsigned int cGameEntity::m_nextID = cGameEntity::STARTING_ID_NUMBER;

unsigned int cGameEntity::getUniqueID(void)
{
	return this->m_uniqueID;
}



// **************************************************************
//	  _ ___ _           _         _  ___  _     _        _   
//	 (_) _ \ |_ _  _ __(_)__ __ _| |/ _ \| |__ (_)___ __| |_ 
//	 | |  _/ ' \ || (_-< / _/ _` | | (_) | '_ \| / -_) _|  _|
//	 |_|_| |_||_\_, /__/_\__\__,_|_|\___/|_.__// \___\__|\__|
//				|__/                         |__/            
void cGameEntity::SetPhysState( cPhysicalProperties &PhysState )
{
	// Do we have a physical state? 
	if ( this->m_pPhysProps )
	{	
		// Nope
		this->m_pPhysProps = new cPhysProps();
		// Set the physical state ID to match the game object
		this->m_pPhysProps->m_GameEntityUniqueID = this->m_uniqueID;
	}

	// Check to see that the ID match...
	if ( PhysState.m_GameEntityUniqueID == this->m_uniqueID )
	{
		// Update it, baby
		(*this->m_pPhysProps) = PhysState;
	}
	// Don't.
	return;
}

// Returns false if there isn't any physical state on this object
bool cGameEntity::HasPhysState(void)
{
	if ( this->m_pPhysProps )
	{
		return true;
	}
	return false;
}

bool cGameEntity::GetPhysState( cPhysicalProperties &PhysState )
{
	if ( this->m_pPhysProps )
	{
		// Copy state to 'return'
		PhysState = (*this->m_pPhysProps);
		return true;
	}
	// There isn't a physical state
	return false;
}
// **************************************************************