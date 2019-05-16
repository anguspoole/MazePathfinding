#include "cLowPassFilter.h"


cLowPassFilter::cLowPassFilter()
{
	this->m_bIsFirstValue = true;

	this->m_Values.reserve(cLowPassFilter::DEFAULT_LENGTH);
	for ( unsigned int index = 0; index != cLowPassFilter::DEFAULT_LENGTH; index++ )
	{
		this->m_Values.push_back( 0.0 );
	}

	this->m_nextIndex = 0;
	return;
};

double cLowPassFilter::getAverage(void)
{
	double total = 0.0f;
	unsigned int arraySize = static_cast<unsigned int>( this->m_Values.size() );
	for ( unsigned int index = 0; index != arraySize; index++ )
	{
		total += this->m_Values[index];
	}
	// Length of vector can never be zero; min length is 1
	// (so we can't have a divide by zero)
	return total / static_cast<double>(arraySize);
}

void cLowPassFilter::setLength(unsigned int newLength)
{
	// Make length of 1 if passed length of zero
	// (So we don't get a DIV by zero when sampling)
	if ( newLength == 0 )
	{	
		newLength = 1;
	}

	double average = this->getAverage();
	this->m_Values.resize( newLength, average );

	return;
}

void cLowPassFilter::AddValue(double value)
{
	if ( this->m_bIsFirstValue )
	{	// Load entire vector with this initial value
		unsigned int arraySize = static_cast<unsigned int>(this->m_Values.size());
		for ( unsigned int index = 0; index != arraySize; index++ )
		{
			this->m_Values[index] = value;
		}
	}//if ( this->m_bIsFirstValue )

	this->m_Values[this->m_nextIndex] = value;

	this->m_nextIndex++;

	if ( this->m_nextIndex >= static_cast<unsigned int>(this->m_Values.size()) )
	{	// reset index
		this->m_nextIndex = 0;
	}
	return;
}

unsigned int cLowPassFilter::getLength(void)
{
	return static_cast<unsigned int>( this->m_Values.size() );
}


