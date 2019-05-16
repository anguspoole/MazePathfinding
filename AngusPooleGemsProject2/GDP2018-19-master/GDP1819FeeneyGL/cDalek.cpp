#include "cDalek.h"

#include <process.h>
#include <Windows.h>		// For CRITICAL_SECTION, CreateThread, ResumeThread, etc.

#include "cRandThreaded.h"
#include "cDalekManagerTripleBuffer.h"

#include <iostream> // Printing to console for debug


//The actual win32 thread function
// EVERY dalek calls this function but on an different thread
DWORD WINAPI DalekBrainThread(void* pInitialData)
{
	// Deref the this pointer to get at the specific Dalek

	cDalek* pDalek = (cDalek*)pInitialData;

	// Delay the start for about 250 ms
	Sleep(250);

	// Now endlessly run the loop until it's time to exit
//	if ( pDalek->bIsAlive )
	while ( true )
	{
	//DEBUG
//		std::cout << "Dalek " << pDalek->getDalekIndex() << " is alive, about to call Update()" << std::endl;
		pDalek->Update();

		// Release control of the thread
		Sleep(1);

	}//pDalek->bIsAlive

	//DEBUG
//	std::cout << "Dalek " << pDalek->getDalekIndex() << " is dead" << std::endl;

	return 0;
}

cDalek::cDalek()
{
	this->bIsAlive = true;
	this->m_dalekIndex = 0;
	this->m_pDalekManager = NULL;

	this->m_position = glm::vec3(0.0f,0.0f,0.0f);
	this->m_Velocity = glm::vec3(0.0f,0.0f,0.0f);

	this->m_pLocalTimer = new CHRTimer();
	this->m_pLocalTimer->ResetAndStart();

	this->m_TimeToMove = 0.0f;

	return;
}

void cDalek::CreateThreadAndStartUpdating( unsigned int dalekIndex, 
										   cDalekManagerTripleBuffer* pDalekManager, 
										   cRandThreaded* pRandThread )
{
	this->m_dalekIndex = dalekIndex;

	this->m_pDalekManager = pDalekManager;

	this->m_pRandThread = pRandThread;

	LPDWORD phThread = 0;	// Clear to zero
	DWORD hThread = 0;	
	HANDLE hThreadHandle = 0;
	
	// Pass a pointer to this instance 
	// Recal that the "this" pointer is the pointer to
	//	this particular instance of the object
	void* pThisDalek = (void*)(this);

	std::cout << "Creating Dalek thread..." << std::endl;
	std::cout.flush();

	this->PickNewDirection();

	hThreadHandle = CreateThread(NULL,	// Default security
								0,		// Stack size - default - win32 = 1 Mbyte
								&DalekBrainThread, // Pointer to the thread's function
								pThisDalek,		// The value (parameter) we pass to the thread
									// This is a pointer to void... more on this evil thing later...
								0,  // CREATE_SUSPENDED or 0 for threads...
								(DWORD*) &phThread);		// pointer or ref to variable that will get loaded with threadID

	return;
}

void cDalek::ResetTimerAndStart(void)
{
	this->m_pLocalTimer->ResetAndStart();
	return;
}

unsigned int cDalek::getDalekIndex(void)
{
	return this->m_dalekIndex;
}


// Will pick a random direction and move for x seconds
void cDalek::PickNewDirection(void)
{
	glm::vec3 newRelativeDestination;

	// Pick a number between -max and +max,on the XZ plane
	const double MAXDISTANCETOMOVE = 20.0;
	newRelativeDestination.x = (float)this->m_pRandThread->getNextRandDoubleInRange( -MAXDISTANCETOMOVE, MAXDISTANCETOMOVE );
	newRelativeDestination.y = 0.0f; //20.0f * ((float)this->m_pRandThread->getNextRandDouble() - 0.5f);
	newRelativeDestination.z = (float)this->m_pRandThread->getNextRandDoubleInRange( -MAXDISTANCETOMOVE, MAXDISTANCETOMOVE );

	// Pick a random time between 4 and 10 seconds
	this->m_TimeToMove = (float)this->m_pRandThread->getNextRandDoubleInRange( 4.0, 10.0 );
	if ( this->m_dalekIndex == 0 )
	{
		std::cout << "New move count down = " << this->m_TimeToMove << std::endl;
	}

	this->m_Velocity = newRelativeDestination / this->m_TimeToMove;

	this->m_pLocalTimer->ResetLongDuration();
	this->m_pLocalTimer->ResetAndStart();

	return;
}


void cDalek::Update(void)
{
//	float deltaTime = this->m_pLocalTimer->GetElapsedSecondsAverage();
	float deltaTime = this->m_pLocalTimer->GetElapsedSeconds(true);

	// Clamp delta time to a reasonable value
//	const float MAXFRAMETIME = 0.1f;	// 100 ms
//	if ( deltaTime > MAXFRAMETIME )
//	{
//		deltaTime = MAXFRAMETIME;
//	}


	glm::vec3 deltaVel = this->m_Velocity * deltaTime;
	
	this->m_position += deltaVel;


	if ( this->m_pLocalTimer->get_fLondDurationTotalSeconds() >= this->m_TimeToMove )
	{
		if ( this->m_dalekIndex == 0 )
		{
			std::cout << "Picking new direction" << std::endl;
		}
		this->PickNewDirection();
	}


	this->m_pDalekManager->UpdatePositionAtIndex( this->m_dalekIndex, this->m_position );

	return;
}
