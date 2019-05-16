#ifndef _cLowPassFilter_HG_
#define _cLowPassFilter_HG_

#include <vector>

class cLowPassFilter
{
public:
	cLowPassFilter();
	void AddValue(double value);
	double getAverage(void);
	void setLength(unsigned int newLength);
	unsigned int getLength(void);

	static const unsigned int DEFAULT_LENGTH = 30;

private:
	std::vector<double> m_Values;
	bool m_bIsFirstValue;
	unsigned int m_nextIndex;
};



#endif