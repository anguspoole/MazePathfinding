#include "cDalekManagerTripleBuffer.h"

#include <process.h>
#include <Windows.h>

CRITICAL_SECTION CSBufferIndex;

cDalekManagerTripleBuffer::cDalekManagerTripleBuffer()
{
	InitializeCriticalSection( &CSBufferIndex );

	// We want the read buffer to be IN FRONT of the write buffer. 
	// So something like this:
	//
	// Buffer 0: write
	// Buffer 1: read
	// Buffer 2: -----
	// 
	// After switch:
	// Buffer 0: (dirty write)
	// Buffer 1: write 
	// Buffer 2: read
	// 
	// The "dirty write" is if some of the threads haven't 
	// realized that the buffers have switched. This is OK 
	// because there not chance of the reader READING bad
	// data, only that the occasional writer is out by one 
	// buffer value. 
	this->m_CurrentWriteBufferIndex = 0;
	this->m_CurrentReadBufferIndex = 1;

	return;
}

cDalekManagerTripleBuffer::~cDalekManagerTripleBuffer()
{
	DeleteCriticalSection( &CSBufferIndex );

	return;
}


void cDalekManagerTripleBuffer::InitBuffers(unsigned int size)
{
	// Create three buffers that are size big...
	// (and fill them will all zeros)
	for ( unsigned int buffIndex = 0; buffIndex != NUMBEROFBUFFERS; buffIndex++ )
	{
		this->vecPositions[buffIndex].reserve(size);

		for ( unsigned int posCount = 0; posCount != size; posCount++ )
		{
			this->vecPositions[buffIndex].push_back( glm::vec3(0.0f,0.0f,0.0f) );
		}
	}

	return;
}


// Called by the renderer (or main thread)
void cDalekManagerTripleBuffer::SwitchBuffers(void)
{
	EnterCriticalSection( &CSBufferIndex );

	this->m_CurrentWriteBufferIndex++;
	this->m_CurrentReadBufferIndex++;

	if ( this->m_CurrentReadBufferIndex >= NUMBEROFBUFFERS )
	{
		this->m_CurrentReadBufferIndex = 0;
	}
	if ( this->m_CurrentWriteBufferIndex >= NUMBEROFBUFFERS )
	{
		this->m_CurrentWriteBufferIndex = 0;
	}

	LeaveCriticalSection( &CSBufferIndex );
	return;
}

unsigned int cDalekManagerTripleBuffer::getCurrentReadBuffer(void)
{
	return this->m_CurrentReadBufferIndex;
}

unsigned int cDalekManagerTripleBuffer::getCurrentWriteBuffer(void)
{
	return this->m_CurrentWriteBufferIndex;
}

//	// Makes a copy from the current read buffer
//void cDalekManagerTripleBuffer::getAllPositions( std::vector<glm::vec3> &vecPositions )
//{
//	vecPositions.clear();
//
//	// Which buffer are we reading from?
//	unsigned int readBufferIndex = this->getCurrentReadBuffer();
//
//	vecPositions.reserve( this->vecPositions[readBufferIndex].size() );
//
//	for ( unsigned int index = 0; index != this->vecPositions[readBufferIndex].size(); index++ )
//	{
//		vecPositions.push_back( this->vecPositions[readBufferIndex][index] );
//	}
//	return;
//}

void cDalekManagerTripleBuffer::getAllPositions( glm::vec3* pPositionArray, unsigned int arraySize )
{
	// Which buffer are we reading from?
	unsigned int readBufferIndex = this->getCurrentReadBuffer();

	// Check that we aren't reading more Daleks than are there.
	if ( arraySize > this->vecPositions[readBufferIndex].size() )
	{
		arraySize = (unsigned int)this->vecPositions[readBufferIndex].size();
	}

	for ( unsigned int index = 0; index != arraySize; index++ )
	{
		pPositionArray[index] = this->vecPositions[readBufferIndex][index];
	}
	return;
}



// This is called by each of the Daleks
void cDalekManagerTripleBuffer::UpdatePositionAtIndex( unsigned int index, 
													   glm::vec3 newPosition )
{
	this->vecPositions[this->m_CurrentWriteBufferIndex][index] = newPosition;

	return;
}
