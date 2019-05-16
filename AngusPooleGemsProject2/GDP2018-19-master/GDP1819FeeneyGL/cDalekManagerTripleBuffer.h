#ifndef _cDalekManagerTripleBuffer_HG_
#define _cDalekManagerTripleBuffer_HG_

#include <glm/vec3.hpp>
#include <vector>

// This class will help manage the rendering of objects
// on the screen, while they are being updated on 
// individual threads. 

class cDalekManagerTripleBuffer
{
public:
	cDalekManagerTripleBuffer();
	~cDalekManagerTripleBuffer();

	void InitBuffers(unsigned int size);

	// Called by the renderer (or main thread)
	void SwitchBuffers(void);

	unsigned int getCurrentReadBuffer(void);
	unsigned int getCurrentWriteBuffer(void);

	// Makes a copy from the current read buffer
//	void getAllPositions( std::vector<glm::vec3> &vecPositions );

	// This is faster than the above, because it doesn't keep re-allocating the vector
	void getAllPositions( glm::vec3* pPositionArray, unsigned int arraySize );

	// This is called by each of the Daleks
	void UpdatePositionAtIndex( unsigned int index, 
								glm::vec3 newPosition );

	// Here is the triple buffer
	// (an array of 3 vectors)
	static const unsigned int NUMBEROFBUFFERS = 3;
	std::vector<glm::vec3> vecPositions[NUMBEROFBUFFERS];

private:
	unsigned int m_CurrentReadBufferIndex;
	unsigned int m_CurrentWriteBufferIndex;
};

#endif
