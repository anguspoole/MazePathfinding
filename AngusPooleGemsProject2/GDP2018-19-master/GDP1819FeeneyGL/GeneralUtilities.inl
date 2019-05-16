
template<class T>
T getRandBetweenRange( T min, T max )
{
	T r = static_cast<T>( static_cast<double>(min) + static_cast<T>( rand() ) / 
		                  ( static_cast <double>(RAND_MAX)/static_cast<double>(max-min) ) );
	return r;
}

template <class T>
void swapValues( T &a, T &b )
{
	T temp = a; 
	a = b;
	b = temp;
	return;
}

template <class T>
T smoothstep(T edge0, T edge1, T x)
{
    // Scale, bias and saturate x to 0..1 range
    x = clamp((x - edge0)/(edge1 - edge0), static_cast<T>(0.0), static_cast<T>(1.0) ); 
    // Evaluate polynomial
    return x*x*(static_cast<T>(3) - static_cast<T>(2)*x);
}

template <class T>
T smootherstep(T edge0, T edge1, T x)
{
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0)/(edge1 - edge0), static_cast<T>(0.0), static_cast<T>(1.0) );
    // Evaluate polynomial
    return x*x*x*(x*(x*static_cast<T>(6) - static_cast<T>(15)) + static_cast<T>(10));
}

template <class T>
T clamp(T x, T lowerlimit, T upperlimit)
{
    if (x < lowerlimit) x = lowerlimit;
    if (x > upperlimit) x = upperlimit;
    return x;
}

template <class T>
T getSign( T num )
{
	if ( num < static_cast<T>(0.0) )	return static_cast<T>(-1.0);
	return static_cast<T>(1);
}
