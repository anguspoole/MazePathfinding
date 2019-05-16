#version 400 core
// geom01.glsl

layout (triangles) in;				// Primitive coming in (can't change)
layout (triangle_strip) out;		// Primitive coming out 
                                    // (can only be strips)
layout (max_vertices = 16) out;		// Will stop if there are more than this number

// Same as in the vertex shader
// (When I set the uniform, these will also be set)
uniform mat4 matModel;		// M
uniform mat4 matModelInvTrans;	// inverse(transpose(matModel))
uniform mat4 matView;		// V
uniform mat4 matProj;		// P


// Coming from the vertex shader
struct sVSOut
{
	vec4 color;		
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
	vec4 vertTanXYZ;	// Tangent to the surface
	vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
};
// Note: this is an array
// of undefined size, as the primitives could be 1 to 6 vertices in size
in sVSOut vsOuput[];	

// Going out to the fragment shader
struct sGSOut
{
	vec4 color;		
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
	vec4 vertTanXYZ;	// Tangent to the surface
	vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
};

out sGSOut gsOuput;

// The geometry shader will only transform the non-skinned mesh
// otherwise it will just be a pass-through shader
uniform bool bIsASkinnedMesh;	// True to do the skinned mesh


// First, this will be a pass-through shader, doing nothing
void main(void)
{
	// Assume that it's 1 triangle coming in, and 1 coming out,
	// with no changes to the triangles
	
	
	if ( bIsASkinnedMesh )
	{
		int i;
		for ( i = 0; i < gl_in.length(); i++ )
		{
			gsOuput.color 			= vsOuput[i].color;
			gsOuput.vertPosWorld	= vsOuput[i].vertPosWorld;
			gsOuput.vertNormal		= vsOuput[i].vertNormal;
			gsOuput.vertUV_x2		= vsOuput[i].vertUV_x2;
			gsOuput.vertTanXYZ		= vsOuput[i].vertTanXYZ;
			gsOuput.vertBiNormXYZ	= vsOuput[i].vertBiNormXYZ;
			
			gl_Position = gl_in[i].gl_Position;
			EmitVertex();
		}

		EndPrimitive();		
	}
	else
	{
		// Output the original model
		int i;
		for ( i = 0; i < gl_in.length(); i++ )
		{
			gsOuput.color 			= vsOuput[i].color;
//			gsOuput.vertPosWorld	= vsOuput[i].vertPosWorld;
//			gsOuput.vertNormal		= vsOuput[i].vertNormal;
			gsOuput.vertUV_x2		= vsOuput[i].vertUV_x2;
			gsOuput.vertTanXYZ		= vsOuput[i].vertTanXYZ;
			gsOuput.vertBiNormXYZ	= vsOuput[i].vertBiNormXYZ;
			
			// This has been taken from the vertex shader
			//  in vertex shader, we have 
			//    gl_Position = vec4(posTemp, 1.0f);
			vec4 posTemp = gl_in[i].gl_Position;
			
			mat4 MVP = matProj * matView * matModel;
			gl_Position = MVP * posTemp;			// ON SCREEN
			gsOuput.vertPosWorld = matModel * posTemp;
			gsOuput.vertNormal = matModelInvTrans * vec4(normalize(vsOuput[i].vertNormal.xyz),1.0f);
			EmitVertex();
		}
		EndPrimitive();		
		
		
//		// Output another set of triangles
//		for ( i = 0; i < gl_in.length(); i++ )
//		{
//			gsOuput.color 			= vsOuput[i].color;
////			gsOuput.vertPosWorld	= vsOuput[i].vertPosWorld;
////			gsOuput.vertNormal		= vsOuput[i].vertNormal;
//			gsOuput.vertUV_x2		= vsOuput[i].vertUV_x2;
//			gsOuput.vertTanXYZ		= vsOuput[i].vertTanXYZ;
//			gsOuput.vertBiNormXYZ	= vsOuput[i].vertBiNormXYZ;
//			
//			// This has been taken from the vertex shader
//			//  in vertex shader, we have 
//			//    gl_Position = vec4(posTemp, 1.0f);
//			vec4 posTemp = gl_in[i].gl_Position;
//			
//			posTemp.x += 0.1f;
//			
//			mat4 MVP = matProj * matView * matModel;
//			gl_Position = MVP * posTemp;			// ON SCREEN
//			gsOuput.vertPosWorld = matModel * posTemp;
//			gsOuput.vertNormal = matModelInvTrans * vec4(normalize(vsOuput[i].vertNormal.xyz),1.0f);
//			EmitVertex();
//		}
//		EndPrimitive();		


	}	
}



