#version 420 
// vertex01.glsl

//uniform mat4 MVP;		
uniform mat4 matModel;		// M
uniform mat4 matModelInvTrans;	// inverse(transpose(matModel))
uniform mat4 matView;		// V
uniform mat4 matProj;		// P

// Coming into the vertex shader
in vec4 vColour;		// rgba   	was "attribute"
in vec4 vPosition;		// xyzw		was "attribute"
in vec4 vNormal;		// normal xyz
in vec4 vUV_x2;			// Texture coordinates (2 of them)
in vec4 vTanXYZ;			// Tangent to the surface
in vec4 vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface
//in float boneID[4];			//unsigned int boneID[4];
//in float boneWeight[4];		//float boneWeights[4];
in vec4 vBoneID;		// really passing it as 4 values
in vec4 vBoneWeight;	

// Going out of the vertex shader
//out vec4 color;			// exit to fragment
//out vec4 vertPosWorld;	// "World space"
//out vec4 vertNormal;	// "Model space"
//out vec4 vertUV_x2;		// To the next shader stage
//out vec4 vertTanXYZ;	// Tangent to the surface
//out vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
struct sVSOut
{
	vec4 color;			// exit to fragment
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
	vec4 vertTanXYZ;	// Tangent to the surface
	vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
};
out sVSOut vsOuput;



// Note, we're in the VERTEX shader now, baby!
uniform sampler2D texHeightMap;
uniform bool bUseHeightMap;			// "turn on" the vertex displacement
uniform float heightMapRatio;		// Increase the range of the displacement

// For skinned mesh
const int MAXNUMBEROFBONES = 100;
uniform mat4 bonesPose1[MAXNUMBEROFBONES];
uniform mat4 bonesPose2[MAXNUMBEROFBONES];

// Blends the entire skinned mesh as one (which might look odd)
uniform float boneBlend;		// 0 to 1  (0 is pose 1, 1 is pose 2)

// Can blend each bone by themselves
uniform mat4 boneBlends[MAXNUMBEROFBONES];


// Pass the acutal number you are using to control how often this loops
uniform int numBonesUsed;			
uniform bool bIsASkinnedMesh;	// True to do the skinned mesh

void main()
{
	// Make a copy of the "model space" vertex
	vec3 posTemp = vPosition.xyz;
	
	// Apply vertex displacement?
	if ( bUseHeightMap )
	{
		// Note: I'm only sampling ONE of the colours, because it's black and white
		// Returns 0.0 to 1.0
		float height = texture( texHeightMap, vUV_x2.st ).r;
		
		height = height * heightMapRatio;
		
		posTemp.y = 0.0f;		// "Flatten" the mesh
		posTemp.y = height;		// Set the heigth
		// You could also "adjust" an existing mesh
	
	}//if ( bUseHeightMap )
	
    vsOuput.color = vColour;
	
	// Pass the texture coordinates out, unchanged.
	vsOuput.vertUV_x2 = vUV_x2;
	
	// Also pass the bi-tangent (bi-normal) and tangent to fragment
	vsOuput.vertTanXYZ = vTanXYZ;		// Tangent to the surface
	vsOuput.vertBiNormXYZ = vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface

	
	// Skinned mesh or not?
	if ( bIsASkinnedMesh )
	{	
		//*****************************************************************
		// Before I do the full MVP (screen space) calculation, 
		// I'm going to adjust where the vertex is based on the 
		// the bone locations

					   
		
		mat4 BoneTransform 
		                = bonesPose1[ int(vBoneID[0]) ] * vBoneWeight[0];
		BoneTransform += bonesPose1[ int(vBoneID[1]) ] * vBoneWeight[1];
		BoneTransform += bonesPose1[ int(vBoneID[2]) ] * vBoneWeight[2];
		BoneTransform += bonesPose1[ int(vBoneID[3]) ] * vBoneWeight[3];
		
		
		mat4 BoneTransform1 
		                = bonesPose1[ int(vBoneID[0]) ] * vBoneWeight[0];
		BoneTransform1 += bonesPose1[ int(vBoneID[1]) ] * vBoneWeight[1];
		BoneTransform1 += bonesPose1[ int(vBoneID[2]) ] * vBoneWeight[2];
		BoneTransform1 += bonesPose1[ int(vBoneID[3]) ] * vBoneWeight[3];
		
		mat4 BoneTransform2 
		                = bonesPose2[ int(vBoneID[0]) ] * vBoneWeight[0];
		BoneTransform2 += bonesPose2[ int(vBoneID[1]) ] * vBoneWeight[1];
		BoneTransform2 += bonesPose2[ int(vBoneID[2]) ] * vBoneWeight[2];
		BoneTransform2 += bonesPose2[ int(vBoneID[3]) ] * vBoneWeight[3];
		
		// Blend the final transformation
		mat4 BoneTransform =   BoneTransform1 * ( 1.0 - boneBlend )
		                     + BoneTransform2 * boneBlend;
		
		// You could also do bBoneID.x, .y, etc.
				
	//	matrixWorld = BoneTransform * matrixWorld;
		
		vec4 vertPositionFromBones = BoneTransform * vec4(posTemp.xyz, 1.0);
		//*****************************************************************
	
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vertPositionFromBones;			// ON SCREEN
		
		vsOuput.vertPosWorld = matModel * vec4(posTemp, 1.0);
		
		
		
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		
		// This only leaves rotation (translation and scale are removed)
		mat4 matBoneTransform_InTrans = inverse(transpose(BoneTransform));
		
		vec4 vNormBone = matBoneTransform_InTrans * vec4(normalize(vNormal.xyz),1.0f);
		
		vsOuput.vertNormal = matModelInvTrans * vNormBone;
		
		
		// And then you do the same for the binormal and bitangent
		vec4 vTanXYZ_Bone = matBoneTransform_InTrans * vTanXYZ;
		vsOuput.vertTanXYZ = matModelInvTrans * vTanXYZ_Bone;
		
		vec4 vBiNormXYZ_Bone = matBoneTransform_InTrans * vBiNormXYZ;
		vsOuput.vertBiNormXYZ = matModelInvTrans * vBiNormXYZ_Bone;
		
		
		// Debug: make green if this flag is set
		vsOuput.color.rgb = vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		// Is a regular mesh
//		// Note these are 'backwards'
//		mat4 MVP = matProj * matView * matModel;
//		gl_Position = MVP * vec4(posTemp, 1.0f);			// ON SCREEN
//		
//		vsOuput.vertPosWorld = matModel * vec4(posTemp, 1.0f);
//		
//		// Transforms the normal into "world space"
//		// Remove all scaling and transformation from model
//		// Leaving ONLY rotation... 
//		vsOuput.vertNormal = matModelInvTrans * vec4(normalize(vNormal.xyz),1.0f);

		// This is being transformed in the geometry shader, now
		// 	so DON'T transform here
		gl_Position = vec4(posTemp, 1.0f);			// ON SCREEN
		
		vsOuput.vertNormal = vNormal;
	
	}//if ( bIsASkinnedMesh )
	
	
}



