#ifndef _cDalek_HG_
#define _cDalek_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <Windows.h>		// For CRITICAL_SECTION, CreateThread, ResumeThread, etc.
#include <process.h>

//#include "CHRTimer.h"
#include "Ply/CHRTimer.h"

// Forward declare 
class cDalekManagerTripleBuffer;
class cRandThreaded;

class cDalek
{
public:
	cDalek();

	// This would create a thread for this Dalek, 
	//	which would call update on this instance
	// The index is so that it can call the triple buffer thing
	void CreateThreadAndStartUpdating( unsigned int dalekIndex, 
									   cDalekManagerTripleBuffer* pDalekManager, 
									   cRandThreaded* pRandThread );


	// The entire update of whatever it is that the Dalek
	//	is trying to (move, look at things in the world)
	void Update(void);

	// When set to false, the thread will exit, stopping any updates
	bool bIsAlive;

	void ResetTimerAndStart(void);

	// Will pick a random direction and move for x seconds
	void PickNewDirection(void);

	unsigned int getDalekIndex(void);

private:
	glm::vec3 m_position;

	unsigned int m_dalekIndex;

	cDalekManagerTripleBuffer* m_pDalekManager;

	cRandThreaded* m_pRandThread;

	// Object will move for this amount of time
	float m_TimeToMove;
	glm::vec3 m_Velocity;

	CHRTimer* m_pLocalTimer;

};						


#endif

