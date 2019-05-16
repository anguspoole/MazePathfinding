#version 420 
//fragment01.glsl

// In from a previous stage (vertex shader)
//in vec4 color;			// in from the vertex shader
//in vec4 vertPosWorld;
//in vec4 vertNormal;		// "Model space" (only rotation)
//in vec4 vertUV_x2;		// Texture coordinates
//in vec4 vertTanXYZ;			// Tangent to the surface
//in vec4 vertBiNormXYZ;		// bi-normal (or bi-tangent) to the surface

//struct sVSOut
//{
//	vec4 color;			// exit to fragment
//	vec4 vertPosWorld;	// "World space"
//	vec4 vertNormal;	// "Model space"
//	vec4 vertUV_x2;		// To the next shader stage
//	vec4 vertTanXYZ;	// Tangent to the surface
//	vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
//}; 
//in sVSOut vsOuput;
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

in sGSOut gsOuput;


uniform vec4 objectDiffuse;		// becomes objectDiffuse in the shader
uniform vec4 objectSpecular;	// rgb + a, which is the power)

uniform vec3 eyeLocation;		// This is in "world space"

// Set this to true (1), and the vertex colour is used
uniform bool useVertexColour;

// If you want this, go ahead and use this.
// You would:
// - Make an "ambient" value that's diffuse * diffuseToAmbientRatio
// - Then, calculate the final colour with ONLY lighting.
// - If this value is LESS than the ambient, use the ambient.
uniform float diffuseToAmbientRatio;		// 0.2 

uniform bool bDontUseLighting;		

uniform bool bIsLightVolume; 	// Used in 2nd pass

// If this is true, then we are using the smoke imposter
// (the shader will be changed a little)
uniform bool bIsParticleImposter;
// Used to fade the particle during "death" (set to 1.0f if not being used)
uniform float ParticleImposterBlackThreshold;
uniform float ParticleImposterAlphaOverride;	


//vec4 gl_FragColor
//out vec4 finalOutputColour;		// Any name, but must be vec4
//out vec4 finalOutputColour[3];		// Any name, but must be vec4
	// Colour was #0
	// Normal was #1
	// Vertex World Location #2
// Or list them in the order that they will be used
out vec4 finalOutputColour;			// GL_COLOR_ATTACHMENT0
out vec4 finalOutputNormal;			// GL_COLOR_ATTACHMENT1
out vec4 finalOutputVertWorldPos;	// GL_COLOR_ATTACHMENT2
out vec4 finalOutputSpecular;		// GL_COLOR_ATTACHMENT3

//struct sOutput
//{
//	vec4 Colour;
//	vec4 Normal;
//	vec4 vertPos;
//};
//out sOutput finalOutoutData;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 10;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms

// CAN'T put texture samplers into an array (sadly)
//uniform sampler textures[10];

uniform sampler2D texture00;
uniform sampler2D texture01;
uniform sampler2D texture02;
uniform sampler2D texture03;
uniform sampler2D texture04;
uniform sampler2D texture05;
uniform sampler2D texture06;
uniform sampler2D texture07;

// For the 2 pass rendering
uniform float renderPassNumber;	// 1 = 1st pass, 2nd for offscreen to quad
uniform sampler2D texPass1OutputColour;
uniform sampler2D texPass1OutputNormal;
uniform sampler2D texPass1OutputVertWorldPos;
uniform sampler2D texPass1OutputSpecular;


// Cube map texture (NOT a sampler3D)
uniform samplerCube textureSkyBox;
uniform bool useSkyBoxTexture;

// 
uniform bool bAddReflect;		// Add reflection
uniform bool bAddRefract;		// Add refraction
uniform float refractionIndex;

// This is 4 x 2 floats or 8 floats
uniform vec4 texBlendWeights[2];	// x is 0, y is 1, z is 2

uniform float wholeObjectAlphaTransparency;

uniform float screenWidth;
uniform float screenHeight;

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

void main()
{
	// output black to all layers
	finalOutputColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputNormal = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputVertWorldPos.xyz = gsOuput.vertPosWorld.xyz;
	finalOutputVertWorldPos.a = 1.0f;
	finalOutputSpecular = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	const float PASS_1_SPECULAR_ADJUSTMENT_RATIO = 1.0f/100.0f;
	const float PASS_2_SPECULAR_ADJUSTMENT_RATIO = 100.0f;
//	const float PASS_1_SPECULAR_ADJUSTMENT_RATIO = 1.0f;
//	const float PASS_2_SPECULAR_ADJUSTMENT_RATIO = 1.0f;

	if ( int(renderPassNumber) == 1 )
	{
		
		// is this the skybox texture?
		if (useSkyBoxTexture == true)
		{
			// Note for cube maps, the texture coordinates are 3D
			// (so here we are using the normal on the surface 
			//  of the sphere, like a "ray cast" really)
			vec3 skyPixelColour = texture( textureSkyBox, gsOuput.vertNormal.xyz ).rgb;
			
			finalOutputColour.rgb = skyPixelColour;
			//finalOutputColour.rgb = vec3(1.0f, 0.0f, 0.0f);
			finalOutputColour.a = 1.0f;
			
			finalOutputNormal.rgb = gsOuput.vertNormal.xyz;
			// Value of 0 in the w of normal means don't use lighting
			finalOutputNormal.a = 0.0f;
			
			finalOutputVertWorldPos.xyz = gsOuput.vertPosWorld.xyz;
			finalOutputVertWorldPos.a = 1.0f;
			
			finalOutputSpecular = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
			return;
		}

		vec4 materialDiffuse = vec4(0.0f,0.0f,0.0f,1.0f);

		if ( useVertexColour )
		{
			//gl_FragColor = vec4(color, 1.0);
			materialDiffuse = gsOuput.color;
		}
		else
		{
			vec4 tex0Col = texture( texture00, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex1Col = texture( texture01, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex2Col = texture( texture02, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex3Col = texture( texture03, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex4Col = texture( texture04, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex5Col = texture( texture05, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex6Col = texture( texture06, gsOuput.vertUV_x2.st ).rgba;
			vec4 tex7Col = texture( texture07, gsOuput.vertUV_x2.st ).rgba;
			
			materialDiffuse =  objectDiffuse
							  + (tex0Col * texBlendWeights[0].x) 	 // 0
							  + (tex1Col * texBlendWeights[0].y)     // 1
							  + (tex2Col * texBlendWeights[0].z)     // 2
							  + (tex3Col * texBlendWeights[0].w)     // 3
							  + (tex4Col * texBlendWeights[1].x)     // 4
							  + (tex5Col * texBlendWeights[1].y)     // 5
							  + (tex6Col * texBlendWeights[1].z)     // 6
							  + (tex7Col * texBlendWeights[1].w);    // 7
		
		}//if ( useVertexColour )	
		
		finalOutputColour = materialDiffuse;
		finalOutputColour.a = 1.0f;
		
		finalOutputNormal = gsOuput.vertNormal;
		if ( bDontUseLighting )
		{
			// w or alpha value of 0.0 means don't apply lighting in 2nd pass
			finalOutputNormal.a = 0.0f;
		}
		else
		{
			finalOutputNormal.a = 1.0f;
		}
		
//		finalOutputVertWorldPos = gsOuput.vertPosWorld;
//		finalOutputVertWorldPos.a = 1.0f;
		
		// Only draw the specular if it's facing towards the eye
		//vec3 eyeVector = normalize(eyeLocation.xyz - gsOuput.vertPosWorld.xyz );

		finalOutputSpecular.rgb = objectSpecular.rgb * PASS_1_SPECULAR_ADJUSTMENT_RATIO; //* eyeToVecDot;
		finalOutputSpecular.a = objectSpecular.a * PASS_1_SPECULAR_ADJUSTMENT_RATIO; //* eyeToVecDot;
//		finalOutputSpecular = vec4( 0.0f, 1.0f, 0.0f, 0.0f );

//		finalOutputSpecular = vec4( 1.0f, 0.0f, 1.0f, 0.0f );

		
		return;
	}

	// Are we in the 2nd pass? 
	if ( int(renderPassNumber) == 2 )
	{ 
	
				
		// Generate the texture coordinates from the fragment location on screen
		vec2 textCoords = vec2( gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight );
//		finalOutputColour.rgb = texture( texPass1OutputColour, textCoords.st ).rgb;
		vec3 vertexMaterialColour = texture( texPass1OutputColour, textCoords.st ).rgb;
		
//		finalOutputColour.rgb = texture( texPass1OutputNormal, textCoords.st ).rgb;
		vec4 vertexNormal = texture( texPass1OutputNormal, textCoords.st );
		
//		finalOutputColour.rgb = texture( texPass1OutputVertWorldPos, textCoords.st ).rgb;
		vec3 vertexWorldPos = texture( texPass1OutputVertWorldPos, textCoords.st ).rgb;

//		finalOutputColour.rgb = texture( texPass1OutputSpecular, textCoords.st ).rgb;
		vec4 vertexSpecular = texture( texPass1OutputSpecular, textCoords.st ).rgba;
		vertexSpecular *= PASS_2_SPECULAR_ADJUSTMENT_RATIO;
		
		// If the w value of the normal is 0.0f, then just use colour (no lighting)
		if ( vertexNormal.w == 0.0f )
		{
			finalOutputColour.rgb = vertexMaterialColour;
		}
		else
		{
			finalOutputColour.rgb = calcualteLightContrib( vertexMaterialColour.rgb, 
														   vertexNormal.xyz, 
														   vertexWorldPos.xyz, 
														   vertexSpecular ).rgb;
		//		finalOutputColour.rgb = calcualteLightContrib( vertexMaterialColour.rgb, 
		//		                                               vertexNormal.xyz, 
		//													   vertexWorldPos.xyz, 
		//													   vec4(1.0f, 0.0f, 0.0f, 1.0f) ).rgb;
		}

		finalOutputColour.a = 1.0f;
		return;
	}
	
	
	// Are we in the 3rd (effects) pass? 
	if ( int(renderPassNumber) == 3 )
	{
		// Generate the texture coordinates from the fragment location on screen
		vec2 textCoords = vec2( gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight );
		vec3 vertexMaterialColour = texture( texPass1OutputColour, textCoords.st ).rgb;
	
		// Effect here
		// Blurring, bloom,. chromo
//		vec3 vertSamp1 = texture( texPass1OutputColour, textCoords.st ).rgb;		
//		vec3 vertSamp2 = texture( texPass1OutputColour, textCoords.st + vec2(0.1, 0.1)).rgb;
//		finalOutputColour.rgb = (vertSamp1 + vertSamp2) / 2.0f;
		
		
		// * * * 			* 
		// * * *          * * *  
		// * * *            *
//		float step = 0.01f;
//		float size = 0.03f;
//		
//		vec3 vecSample = vec3(0.0f, 0.0f, 0.0f );
//		int count = 0;
//		
//		for ( float x = -size; x <= size; x += step )
//		{
//			for ( float y = -size; y <= size; y += step )
//			{
//				count++;
//				vecSample += texture( texPass1OutputColour, textCoords.st + vec2(x, y)).rgb;
//			}
//		}
//		
//	
//		finalOutputColour.rgb = (vecSample.rgb / float(count));
	
	
		finalOutputColour.rgb = vertexMaterialColour.rgb;
		finalOutputColour.a = 1.0f;
		
		return;
	}
	
}

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
			
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)

			// TODO: Still need to do specular, but this gives you an idea
			finalOutputColour.rgb = finalObjectColour.rgb;
//			finalOutputColour.a = wholeObjectAlphaTransparency;

			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
					   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb * lightSpecularContrib.rgb );
//		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
//								  +  lightSpecularContrib.rgb;
		// ********************************************************
	}//for(intindex=0...
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}
