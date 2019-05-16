#ifndef _sPlyVertex_HG_
#define _sPlyVertex_HG_

struct sPlyVertex 
{
	sPlyVertex() : 
		x(0.0f), y(0.0f), z(0.0f),
		nx(0.0f), ny(0.0f), nz(0.0f), 
		u(0.0f), v(0.0f), 
		r(0.0f), g(0.0f), b(0.0f), a(1.0f) {};
	float x;
	float y;
	float z;
	// Now has normals (this is FROM THE FILE)
	float nx;
	float ny;
	float nz;

	float u, v;		// Load a single UV (texture coordinate)

	float r, g, b, a;
};

#endif 
