#ifndef _GeneralUtilities_HG_
#define _GeneralUtilities_HG_

#include <glm/vec3.hpp>

#include "cLowPassFilter.h"

template<class T>	T getRandBetweenRange( T min, T max );

template <class T>	void swapValues( T &a, T &b );

template <class T>	T smoothstep(T edge0, T edge1, T x);

template <class T>	T smootherstep(T edge0, T edge1, T x);

template <class T>	T clamp(T x, T lowerlimit, T upperlimit);

template <class T>	T getSign( T num );

glm::vec3 getRandRGBColour(void);
glm::vec3 getRandRGBColour(glm::vec3 minRGB, glm::vec3 maxRGB );


// External template
#include "GeneralUtilities.inl"

#endif
