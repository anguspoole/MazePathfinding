#ifndef _iPhysicalObject_HG_
#define _iPhysicalObject_HG

#include "cPhysicalProperties.h"

// This is the cGameEntity interface that the "physics" updater can "see"
//	into the cGameEntity without knowing all about the cGameObject.
// When multi-threaded, this is used to 'extract' the physical properties, to update, then return.

// Pure virtual interface
class iPhysicalObject
{
public:

	virtual ~iPhysicalObject() {};			// Make a virtual destructor on any interface (prevents memory leak)
	// Note the virtual and the "= 0". If ALL of these 
	// methods have this, then it's a "pure virtual" or 
	// "interface class"
	virtual void SetPhysState( cPhysicalProperties &PhysState ) = 0;
	// Returns false if there isn't any physical state on this object
	virtual bool GetPhysState( cPhysicalProperties &PhysState ) = 0;
	virtual bool HasPhysState(void) = 0;
};



#endif
