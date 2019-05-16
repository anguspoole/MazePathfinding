#include "globalOpenGLStuff.h"
#include "globalStuff.h"		// for g_pRogerRabbit

#include "cVAOMeshManager.h"
#include "cMeshSceneObject.h"

#include "DebugRenderer/cDebugRenderer.h"

#include <iostream>

#include "sModelDrawInfo.h"	

#include "cAnimationState.h"

CRITICAL_SECTION CS_Models;

// Skinned mesh object 
// Declared in globalStuff.h
//extern cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh = NULL;


// Here's the 'Assimp to VAO Converer" thing....
bool AssimpSM_to_VAO_Converter( cSimpleAssimpSkinnedMesh* pTheAssimpSM,
								unsigned int shaderProgramID )
{
	sModelDrawInfo theSMDrawInfo;

	theSMDrawInfo.meshFileName = pTheAssimpSM->fileName;
	theSMDrawInfo.friendlyName = pTheAssimpSM->friendlyName;

	// Copy the data from assimp format into the sModelDrawInfo format...

	theSMDrawInfo.numberOfVertices = pTheAssimpSM->pScene->mMeshes[0]->mNumVertices;
	theSMDrawInfo.numberOfTriangles = pTheAssimpSM->pScene->mMeshes[0]->mNumFaces;

	// We used the "triangulate" option when loading so all the primitives
	//	will be triangles, but BEWARE!
	theSMDrawInfo.numberOfIndices = theSMDrawInfo.numberOfTriangles * 3;

	
	// Allocate the vertex array (it's a c-style array)
	theSMDrawInfo.pMeshData = new cMesh();

	theSMDrawInfo.pMeshData->pVertices = new sVertex_xyz_rgba_n_uv2_bt_4Bones[theSMDrawInfo.numberOfVertices];
	
	// Danger Will Robinson! 
	// You don't really need to do this, but at least it will clear it to zero.
	// (compiler will complain that it's 'not safe', etc.)
	memset( theSMDrawInfo.pMeshData->pVertices, 0, 
			sizeof(sVertex_xyz_rgba_n_uv2_bt_4Bones) * theSMDrawInfo.numberOfVertices );


	for ( unsigned int vertIndex = 0; vertIndex != theSMDrawInfo.numberOfVertices; vertIndex++ )
	{
		theSMDrawInfo.pMeshData->pVertices[vertIndex].x
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].y
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].z
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].z;

		// Normals... 
		theSMDrawInfo.pMeshData->pVertices[vertIndex].nx
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].ny
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].nz
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].z;

		// Colours... 
		// (If there are no colours, make it hit pink)
		// Note: the method is because you could have more than one set of 
		//	vertex colours in the model (Why? Who the heck knows?) 
		if ( pTheAssimpSM->pScene->mMeshes[0]->HasVertexColors(0) )
		{
			theSMDrawInfo.pMeshData->pVertices[vertIndex].r
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->r;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].g
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->g;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].b
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->b;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].a
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->a;
		}
		else
		{	// hotpink	#FF69B4	rgb(255,105,180)
			theSMDrawInfo.pMeshData->pVertices[vertIndex].r = 1.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].g = 105.0f/255.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].b = 180.0f/255.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].a = 1.0f;
		}

		//bi-normal  (or bi-tangent)
		theSMDrawInfo.pMeshData->pVertices[vertIndex].bx
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].by
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].bz
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].z;

		// Tangent
		theSMDrawInfo.pMeshData->pVertices[vertIndex].tx
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].ty
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].tz
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].z;
		
		// uv2 (which are odd in assimp)
		// Note that there is an array of texture coordinates, 
		// up to 8 (actually). Usually, there's only 1
		if ( pTheAssimpSM->pScene->mMeshes[0]->HasTextureCoords(0) )	// 1st UV coords
		{
			// Assume there's 1... (at least)
			theSMDrawInfo.pMeshData->pVertices[vertIndex].u0 
			= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[0]->x;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].v0
			= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[0]->y;
		}
		if ( pTheAssimpSM->pScene->mMeshes[0]->HasTextureCoords(1) )	// 2nd UV coords
		{
			theSMDrawInfo.pMeshData->pVertices[vertIndex].u0 
			= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[1]->x;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].v0
			= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[1]->y;
		}
		// TODO: add additional texture coordinates (mTextureCoords[1], etc.)


		// 4Bones: ids and weights
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[0] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[0];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[1] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[1];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[2] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[2];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[3] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[3];

		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[0] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[0];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[1] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[1];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[2] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[2];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[3] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[3];

		
	}//for ( unsigned int vertIndex = 0;

	// And the triangles

	// Allocate the array to hold the indices (triangle) info


	// Allocate the array for that (indices NOT triangles)
	theSMDrawInfo.pMeshData->pIndices = new unsigned int[theSMDrawInfo.numberOfIndices];
	
	// Danger Will Robinson! 
	// You don't really need to do this, but at least it will clear it to zero.
	// (compiler will complain that it's 'not safe', etc.)
	memset( theSMDrawInfo.pMeshData->pIndices, 0, 
			sizeof(unsigned int) * theSMDrawInfo.numberOfIndices );


	unsigned int numTriangles = pTheAssimpSM->pScene->mMeshes[0]->mNumFaces;
	unsigned int triIndex = 0;		// Steps through the triangles.
	unsigned int indexIndex = 0;	// Setps through the indices (index buffer)
	for ( ; triIndex != theSMDrawInfo.numberOfTriangles; 
		  triIndex++, indexIndex += 3 )	// Note, every 1 triangle = 3 index steps
	{
		// Get the triangle at this triangle index...
		aiFace* pAIFace = &(pTheAssimpSM->pScene->mMeshes[0]->mFaces[triIndex]);

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 0]				// Offset by 0 (zero)
			= pAIFace->mIndices[0];	// vertex 0

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 1]				// Offset by 1
			= pAIFace->mIndices[1];	// vertex 1

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 2]				// Offset by 2
			= pAIFace->mIndices[2];	// vertex 1
	}//for ( ; triIndex != numVertices; 


	// Calculate the extents on the mesh
	// (Note, because I'm a bone head, this is dupicated...)
	theSMDrawInfo.pMeshData->name = theSMDrawInfo.friendlyName;
	theSMDrawInfo.pMeshData->numberOfIndices = theSMDrawInfo.numberOfIndices;
	theSMDrawInfo.pMeshData->numberOfTriangles = theSMDrawInfo.numberOfTriangles;
	theSMDrawInfo.pMeshData->numberOfVertices = theSMDrawInfo.numberOfVertices;
	theSMDrawInfo.pMeshData->CalculateExtents();

	// ...then pass it to the mesh manager

	::g_pTheVAOMeshManager->LoadModelIntoVAO( theSMDrawInfo, shaderProgramID );
	return true;
}




//void LoadSkinnedMeshModel( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw, 
//						   GLuint shaderProgramID )
//{ 
//	::g_pRPGSkinnedMesh = new cSimpleAssimpSkinnedMesh();
//
//	//if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/modelsMD5monsters/hellknight/attack2.md5anim" ) ) 
//	//{
//	//	std::cout << "Didn't load the army pilot" << std::endl;
//	//}
//
//	//if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/modelsFBX/ArmyPilot(FBX2013).fbx" ) ) 
//	//{
//	//	std::cout << "Didn't load the army pilot" << std::endl;
//	//}
//	//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsMD5/hellknight/attack2.md5anim" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" ) ) 
//	if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "RPG-Character", "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) )
//	{
//		std::cout << "Error: problem loading the skinned mesh" << std::endl;
//	}
//	std::vector<std::string> vecBoneNames;
//	::g_pRPGSkinnedMesh->GetListOfBoneIDandNames( vecBoneNames );
//
//	// Now load another animation file... 
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Idle", "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Unarmed-Attack-Kick-L1", "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Unarmed-Walk", "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Unarmed-Fall", "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Roll-Backward", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Roll-Forwards", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Roll-Left", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Roll-Right", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX" );
//	::g_pRPGSkinnedMesh->LoadMeshAnimation( "Unarmed-Jump", "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX" );
//
//	::g_pRPGSkinnedMesh->friendlyName = "RPG-Character";
//
//	cMesh* pTheMesh = ::g_pRPGSkinnedMesh->CreateMeshObjectFromCurrentModel();
//
//	if ( pTheMesh )
//	{
//		std::cout << "Mesh got loaded" << std::endl;
//	}
//	else
//	{
//		std::cout << "Didn't load the skinned mesh model" << std::endl;
//	}
//
//	// Copy the mesh information from assimp into our cMesh object, 
//	// then into the sModelDrawInfo thing, and pass to the VAOManager
//
//	if ( ! AssimpSM_to_VAO_Converter( ::g_pRPGSkinnedMesh, shaderProgramID ) )
//	{
//		std::cout << "Error: Didn't copy the skinned mesh into the VAO format." << std::endl;
//	}
//	else
//	{
//		std::cout << "Copied the skinned mesh into the VAO format" << std::endl;
//
//		// Add this mesh model into the "models to draw" vector
//
//		{	// Bind pose Skinned Mesh object
//			cMeshSceneObject* pTestSM = new cMeshSceneObject();
//			pTestSM->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 0.0f ) );	// Yellow
//			pTestSM->setAlphaTransparency( 1.0f );
//			pTestSM->friendlyName = "SM_Object";
//			pTestSM->position = glm::vec3(0.0f, 0.0f, 0.0f);
//			pTestSM->setUniformScale(1.0f);
////			pTestSM->meshName = "RPG-Character.ply";	
//			pTestSM->vecLODMeshs.push_back(sLODInfo("RPG-Character.fbx"));
//			
//			// Wireframe, to make it easier to see (for now)
////			pTestSM->bIsWireFrame = true;
////			pTestSM->bDontLight = true;
//			
//			// Set this mesh to the skinned mesh object
//			pTestSM->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
//			// HACK
//			//pTestSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx";
////			pTestSM->currentAnimation = "Idle";
//
//			cAnimationState* pAniState;
//			pTestSM->pAniState = new cAnimationState();
//
//			pTestSM->pAniState->defaultAnimation.name = "Idle";
//			
//			sTextureInfo testObjectTexture;
//			testObjectTexture.name = "grass.bmp";
//			testObjectTexture.strength = 1.0f;
//
//			pTestSM->vecTextures.push_back( sTextureInfo(testObjectTexture) );
//			pTestSM->setUniformScale(1.0f);
//			vec_pObjectsToDraw.push_back( pTestSM );
//		}		
//	}//if ( ! AssimpSM_to_VAO_Converter(
//
//	//if ( pTheMesh )
//	//{
//	//	if ( ! pVAOManager->loadMeshIntoVAO( *pTheMesh, shaderID, false ) )
//	//	{
//	//		std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
//	//	}
//	//}
//	//else
//	//{
//	//	std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
//	//}
//	//// Delete temporary mesh if still around
//	//if ( pTheMesh )
//	//{
//	//	delete pTheMesh;
//	//}
//
//
//	return;
//}

void LoadSkinnedMeshModelTypes( cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID )
{
	// RPG character

	cSimpleAssimpSkinnedMesh* pRPG = new cSimpleAssimpSkinnedMesh();

	if ( ! pRPG->LoadMeshFromFile( "RPG-Character", "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) )
	{
		std::cout << "Error: problem loading the skinned mesh" << std::endl;
	}

	// Now load another animation file... 
	pRPG->LoadMeshAnimation( "Idle", "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx" );
	pRPG->LoadMeshAnimation( "Unarmed-Attack-Kick-L1", "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Unarmed-Walk", "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Unarmed-Fall", "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" );
	pRPG->LoadMeshAnimation( "Roll-Backward", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx" );
	pRPG->LoadMeshAnimation( "Roll-Forwards", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Roll-Left", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Roll-Right", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Unarmed-Jump", "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Unarmed-Attack-L3", "assets/modelsFBX/RPG-Character_Unarmed-Attack-L3(FBX2013).FBX" );
	pRPG->LoadMeshAnimation( "Unarmed-Attack-R3", "assets/modelsFBX/RPG-Character_Unarmed-Attack-R3(FBX2013).FBX" );

	pRPG->friendlyName = "RPG-Character";

	if ( ! pTheVAOMeshManager->LoadSkinnedMeshModelIntoVAO( pRPG, shaderProgramID ) )
	{
		std::cout << "ERROR: Failed to load RPG Skinned Mesh character" << std::endl;
	}
	else
	{
		std::cout << "RPG SM character + animations are loaded OK" << std::endl;
	}

	std::vector<std::string> vecBoneList;
	pRPG->GetListOfBoneIDandNames( vecBoneList );
	for ( int index = 0; index != vecBoneList.size(); index++ )
	{
		//std::cout << index << " : " << vecBoneList[index] << std::endl;
	}

	return;
}

// Loading models was moved into this function
void LoadModelTypes( cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID )
{

	// Load the skinned mesh types into the VAO 
	LoadSkinnedMeshModelTypes( pTheVAOMeshManager, shaderProgramID );


	// Load the cube map
//	::g_pTheTextureManager->SetBasePath("assets/textures/cubemaps");
	std::string errorString;
//	if ( ::g_pTheTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap", 
//		 "SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp", 
//		 "SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp", 
//		 "SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp", true, errorString ) )
	cBasicTextureManager::sCubeMapTextureLoadParams cubeLoadParams;
	cubeLoadParams.cubeMapName = "CityCubeMap";
	cubeLoadParams.posX_fileName = "SpaceBox_right1_posX.bmp";
	cubeLoadParams.negX_fileName = "SpaceBox_left2_negX.bmp";
	cubeLoadParams.posY_fileName = "SpaceBox_top3_posY.bmp";
	cubeLoadParams.negY_fileName = "SpaceBox_bottom4_negY.bmp";
	cubeLoadParams.posZ_fileName = "SpaceBox_front5_posZ.bmp";
	cubeLoadParams.negZ_fileName = "SpaceBox_back6_negZ.bmp";
	cubeLoadParams.basePath = "assets/textures/cubemaps";
	cubeLoadParams.bIsSeamless = true;
	if ( ::g_pTheTextureManager->CreateCubeTextureFromBMPFiles( cubeLoadParams, errorString ) )
	{
		//std::cout << "Loaded the space cube map OK" << std::endl;
	}
	else
	{
		std::cout << "Error: space cube map DIDN't load. On no!" << std::endl;
	}
	//if ( ::g_pTheTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap", 
	//	 "TropicalSunnyDayLeft2048.bmp", "TropicalSunnyDayRight2048.bmp",		// Alternate these
	//	 "TropicalSunnyDayDown2048.bmp", "TropicalSunnyDayUp2048.bmp", 			// Rotate these 90 degrees
	//	 "TropicalSunnyDayFront2048.bmp", "TropicalSunnyDayBack2048.bmp", true, errorString ) )
	//{
	//	std::cout << "Loaded the city cube map OK" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Error: city cube map DIDN't load. On no!" << std::endl;
	//}


		// Load the textures, too
//	::g_pTheTextureManager->SetBasePath("assets/textures");

	// Async load
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Justin.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("grass.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("water-1514818_960_720.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("brick-wall.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("220px-Emma_Watson_2013.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Flowers.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Smoke_1.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Plasma_Ring.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("rock_cave_stylized_height.bmp", true );

	// Async load
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("Justin.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("grass.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("water-1514818_960_720.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("brick-wall.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("220px-Emma_Watson_2013.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("Flowers.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("Smoke_1.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("Plasma_Ring.bmp", "assets/textures"), true );
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC( cBasicTextureManager::s2DTextureLoadParams("rock_cave_stylized_height.bmp", "assets/textures"), true );

	//// load
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Justin.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("grass.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("water-1514818_960_720.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("brick-wall.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("220px-Emma_Watson_2013.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Flowers.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Smoke_1.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Plasma_Ring.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("rock_cave_stylized_height.bmp", "assets/textures"), true );


	//	pTheVAOMeshManager->changeLoaderToOriginal();
	pTheVAOMeshManager->changeLoaderToPly5n();

	pTheVAOMeshManager->SetBasePath("assets/models");



	// Player
	{
		sModelDrawInfo playerBody;
		playerBody.meshFileName = "Player_Body.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerBody, shaderProgramID);

		sModelDrawInfo playerHead;
		playerHead.meshFileName = "Player_Head.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerHead, shaderProgramID);

		sModelDrawInfo playerLeftBottomWing;
		playerLeftBottomWing.meshFileName = "Player_Left_Bottom_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerLeftBottomWing, shaderProgramID);

		sModelDrawInfo playerLeftTopWing;
		playerLeftTopWing.meshFileName = "Player_Left_Top_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerLeftTopWing, shaderProgramID);

		sModelDrawInfo playerRightBottomWing;
		playerRightBottomWing.meshFileName = "Player_Right_Bottom_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerRightBottomWing, shaderProgramID);

		sModelDrawInfo playerRightTopWing;
		playerRightTopWing.meshFileName = "Player_Right_Top_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO(playerRightTopWing, shaderProgramID);
	}

	sModelDrawInfo dalekRes2;
	dalekRes2.meshFileName = "dalek2005_xyz_uv_res_2.ply";	
	if ( ! pTheVAOMeshManager->LoadModelIntoVAO(dalekRes2, shaderProgramID) )
	{
		std::cout << "Didn't load the Dalek, res 2" << std::endl;
		std::cout << pTheVAOMeshManager->GetLastError() << std::endl;
	}

	sModelDrawInfo bunnyInfo;
	//bunnyInfo.meshFileName = "bun_res3_xyz.ply";
	//bunnyInfo.meshFileName = "bun_res3_xyz_n_uv.ply";	// NOW, WITH NORMALS!!
	bunnyInfo.meshFileName = "bun_zipper_hi_res_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	if ( ! pTheVAOMeshManager->LoadModelIntoVAO(bunnyInfo, shaderProgramID) )
	{
		std::cout << "Didn't load the bunny" << std::endl;
		std::cout << pTheVAOMeshManager->GetLastError() << std::endl;
	}

	// And the other LODs
		// bun_zipper_res_2_xyz_n_smartUVBlender.ply
	// bun_zipper_res_3_xyz_n_smartUVBlender.ply
	sModelDrawInfo bunnyInfo_Res2;
	bunnyInfo_Res2.meshFileName = "bun_zipper_res_2_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	pTheVAOMeshManager->LoadModelIntoVAO(bunnyInfo_Res2, shaderProgramID);

	sModelDrawInfo bunnyInfo_Res3;
	bunnyInfo_Res3.meshFileName = "bun_zipper_res_3_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	pTheVAOMeshManager->LoadModelIntoVAO(bunnyInfo_Res3, shaderProgramID);

	sModelDrawInfo gentleTerrain;
	gentleTerrain.meshFileName = "Gentle_Terrain_xyz_uv.ply";			// "ssj100_xyz.ply";
	if ( pTheVAOMeshManager->LoadModelIntoVAO(gentleTerrain, shaderProgramID) )
	{
		//std::cout << "Gentle terrain is loaded" << std::endl;
	}

	sModelDrawInfo airplaneInfo;
	airplaneInfo.meshFileName = "ssj100_xyz_n_GARBAGE_uv.ply";			// "ssj100_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(airplaneInfo, shaderProgramID);

	sModelDrawInfo backToTheFuture;
	backToTheFuture.meshFileName = "de--lorean_xyz_n_GARBAGE_uv.ply";	// "de--lorean.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(backToTheFuture, shaderProgramID);

	sModelDrawInfo migInfo;
	migInfo.meshFileName = "mig29_xyz_n_GARBAGE_uv.ply";					// "mig29_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(migInfo, shaderProgramID);

	sModelDrawInfo teapotInfo;
	teapotInfo.meshFileName = "Utah_Teapot_xyz_n_GARBAGE_uv.ply";			// "Utah_Teapot_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(teapotInfo, shaderProgramID);

	sModelDrawInfo fishInfo;
	fishInfo.meshFileName = "PacificCod0_rotated_xyz_n_GARBAGE_uv.ply";		// "PacificCod0_rotated_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(fishInfo, shaderProgramID);

	sModelDrawInfo mountainInfo;
	mountainInfo.meshFileName = "mountainterrain2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(mountainInfo, shaderProgramID);

	//sModelDrawInfo metropolisInfo;
	//metropolisInfo.meshFileName = "metropolis.ply";		// "mountainterrain2.ply";
	//pTheVAOMeshManager->LoadModelIntoVAO(metropolisInfo, shaderProgramID);

	sModelDrawInfo threeDModInfo;
	threeDModInfo.meshFileName = "3d-modelC.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(threeDModInfo, shaderProgramID);

	sModelDrawInfo asteroidInfo;
	asteroidInfo.meshFileName = "AsteroidStation.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(asteroidInfo, shaderProgramID);

	sModelDrawInfo frigateInfo;
	frigateInfo.meshFileName = "CyclopsFrigate.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(frigateInfo, shaderProgramID);

	sModelDrawInfo heliosInfo;
	heliosInfo.meshFileName = "HeliosFrigate2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(heliosInfo, shaderProgramID);

	sModelDrawInfo destroyerInfo;
	destroyerInfo.meshFileName = "OxossiDestroyer.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(destroyerInfo, shaderProgramID);

	sModelDrawInfo beaconsInfo;
	beaconsInfo.meshFileName = "beacons.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beaconsInfo, shaderProgramID);

	sModelDrawInfo beacons1Info;
	beacons1Info.meshFileName = "beacons1.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons1Info, shaderProgramID);

	sModelDrawInfo beacons2Info;
	beacons2Info.meshFileName = "beacons2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons2Info, shaderProgramID);

	sModelDrawInfo beacons3Info;
	beacons3Info.meshFileName = "beacons3.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons3Info, shaderProgramID);

	sModelDrawInfo beacons4Info;
	beacons4Info.meshFileName = "beacons4.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons4Info, shaderProgramID);

	sModelDrawInfo beacons5Info;
	beacons5Info.meshFileName = "beacons5.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons5Info, shaderProgramID);

	sModelDrawInfo beacons6Info;
	beacons6Info.meshFileName = "beacons6.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons6Info, shaderProgramID);

	sModelDrawInfo beacons7Info;
	beacons7Info.meshFileName = "beacons7.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons7Info, shaderProgramID);

	sModelDrawInfo beacons8Info;
	beacons8Info.meshFileName = "beacons8.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons8Info, shaderProgramID);

	sModelDrawInfo beacons9Info;
	beacons9Info.meshFileName = "beacons9.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons9Info, shaderProgramID);

	sModelDrawInfo beacons10Info;
	beacons10Info.meshFileName = "beacons10.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons10Info, shaderProgramID);

	sModelDrawInfo beacons11Info;
	beacons11Info.meshFileName = "beacons11.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(beacons11Info, shaderProgramID);

	sModelDrawInfo terrainInfo;
	terrainInfo.meshFileName = "MeshLab_Fractal_Terrain_xyz_n_uv.ply";	// "MeshLab_Fractal_Terrain_xyz.ply";
	// Will alow me to update the vertex data in the mesh
	terrainInfo.bVertexBufferIsDynamic = true;

	pTheVAOMeshManager->LoadModelIntoVAO(terrainInfo, shaderProgramID);
	
	sModelDrawInfo cityInfo;
	cityInfo.meshFileName = "ScifiCityRuins.ply";	// "MeshLab_Fractal_Terrain_xyz.ply";
	// Will alow me to update the vertex data in the mesh
	cityInfo.bVertexBufferIsDynamic = true;

	pTheVAOMeshManager->LoadModelIntoVAO(cityInfo, shaderProgramID);
	
	sModelDrawInfo sphereInfo;
	sphereInfo.meshFileName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInfo, shaderProgramID);

	sModelDrawInfo sphereInvertedNormalsInfo;
	sphereInvertedNormalsInfo.meshFileName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInvertedNormalsInfo, shaderProgramID);

	sModelDrawInfo lonelyTri;
	lonelyTri.meshFileName = "singleTriangle_xyz_n.ply";			// "singleTriangle_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(lonelyTri, shaderProgramID);

	sModelDrawInfo cube1x1x1;
	cube1x1x1.meshFileName = "cube_flat_shaded_xyz_n_uv.ply";			// "cube_flat_shaded_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(cube1x1x1, shaderProgramID);


	//sModelDrawInfo oldhouse;
	//oldhouse.meshFileName = "Old_House_Combined_xyz_n.ply";		
	//pTheVAOMeshManager->LoadModelIntoVAO(oldhouse, shaderProgramID);

	sModelDrawInfo skyPirate;
	skyPirate.meshFileName = "Sky_Pirate_Combined_xyz_n_BlenderSmart_uv.ply";		
	if ( pTheVAOMeshManager->LoadModelIntoVAO(skyPirate, shaderProgramID) )
	{
		//std::cout << "Sky Pirate is loaded" << std::endl;
	}

	// At this point, mesh in in GPU
	//std::cout << "Mesh was loaded OK" << std::endl;


	sModelDrawInfo imposterObject_1;
	imposterObject_1.meshFileName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";				// Flat, 2 sided 'classic' imposter
	pTheVAOMeshManager->LoadModelIntoVAO(imposterObject_1, shaderProgramID);

	sModelDrawInfo imposterObject_2;
	imposterObject_2.meshFileName = "1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply";				// More complicated "star" shaped imposter
	pTheVAOMeshManager->LoadModelIntoVAO(imposterObject_2, shaderProgramID);

	sModelDrawInfo imposterObject_3;
	imposterObject_3.meshFileName = "Sphere_UV_xyz_n_uv.ply";							// A simple UV mapped sphere (low poly)
	pTheVAOMeshManager->LoadModelIntoVAO(imposterObject_3, shaderProgramID);

//	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("Justin.bmp", true ) )
//	{
//		std::cout << "Didn't load texture" << std::endl;
//	}
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("grass.bmp", true );
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("water-1514818_960_720.bmp", true );
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("brick-wall.bmp", true );
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("220px-Emma_Watson_2013.bmp", true );
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Flowers.bmp", true );

	// For the particle imposters
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Smoke_1.bmp", true );
//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Plasma_Ring.bmp", true );
//
//	if ( ::g_pTheTextureManager->Create2DTextureFromBMPFile("rock_cave_stylized_height.bmp", true ) )
//	{
//		std::cout << "loaded height map, too" << std::endl;
//	}




	sModelDrawInfo testObject;
	testObject.meshFileName = "SM_Env_Tree_Canopy_04.ply";							// A simple UV mapped sphere (low poly)
	pTheVAOMeshManager->SetBasePath("assets/models/final");
	pTheVAOMeshManager->LoadModelIntoVAO(testObject, shaderProgramID);

	return;
}

// Loading models was moved into this function
void LoadModelTypes_ASYNC(cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID)
{

	// Load the skinned mesh types into the VAO 
	LoadSkinnedMeshModelTypes(pTheVAOMeshManager, shaderProgramID);




	// Load the cube map
//	::g_pTheTextureManager->SetBasePath("assets/textures/cubemaps");
	std::string errorString;
	//	if ( ::g_pTheTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap", 
	//		 "SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp", 
	//		 "SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp", 
	//		 "SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp", true, errorString ) )
	cBasicTextureManager::sCubeMapTextureLoadParams cubeLoadParams;
	cubeLoadParams.cubeMapName = "CityCubeMap";
	cubeLoadParams.posX_fileName = "SpaceBox_right1_posX.bmp";
	cubeLoadParams.negX_fileName = "SpaceBox_left2_negX.bmp";
	cubeLoadParams.posY_fileName = "SpaceBox_top3_posY.bmp";
	cubeLoadParams.negY_fileName = "SpaceBox_bottom4_negY.bmp";
	cubeLoadParams.posZ_fileName = "SpaceBox_front5_posZ.bmp";
	cubeLoadParams.negZ_fileName = "SpaceBox_back6_negZ.bmp";
	cubeLoadParams.basePath = "assets/textures/cubemaps";
	cubeLoadParams.bIsSeamless = true;
	if (::g_pTheTextureManager->CreateCubeTextureFromBMPFiles(cubeLoadParams, errorString))
	{
		//std::cout << "Loaded the space cube map OK" << std::endl;
	}
	else
	{
		std::cout << "Error: space cube map DIDN't load. On no!" << std::endl;
	}
	//if ( ::g_pTheTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap", 
	//	 "TropicalSunnyDayLeft2048.bmp", "TropicalSunnyDayRight2048.bmp",		// Alternate these
	//	 "TropicalSunnyDayDown2048.bmp", "TropicalSunnyDayUp2048.bmp", 			// Rotate these 90 degrees
	//	 "TropicalSunnyDayFront2048.bmp", "TropicalSunnyDayBack2048.bmp", true, errorString ) )
	//{
	//	std::cout << "Loaded the city cube map OK" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Error: city cube map DIDN't load. On no!" << std::endl;
	//}


		// Load the textures, too
//	::g_pTheTextureManager->SetBasePath("assets/textures");

	// Async load
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Justin.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("grass.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("water-1514818_960_720.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("brick-wall.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("220px-Emma_Watson_2013.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Flowers.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Smoke_1.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("Plasma_Ring.bmp", true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC("rock_cave_stylized_height.bmp", true );

	// Async load
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("Justin.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("grass.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("water-1514818_960_720.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("brick-wall.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("220px-Emma_Watson_2013.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("Flowers.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("Smoke_1.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("Plasma_Ring.bmp", "assets/textures"), true);
	::g_pTheTextureManager->Create2DTextureFromBMPFile_ASYNC(cBasicTextureManager::s2DTextureLoadParams("rock_cave_stylized_height.bmp", "assets/textures"), true);

	//// load
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Justin.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("grass.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("water-1514818_960_720.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("brick-wall.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("220px-Emma_Watson_2013.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Flowers.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Smoke_1.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("Plasma_Ring.bmp", "assets/textures"), true );
	//::g_pTheTextureManager->Create2DTextureFromBMPFile( cBasicTextureManager::s2DTextureLoadParams("rock_cave_stylized_height.bmp", "assets/textures"), true );


	//	pTheVAOMeshManager->changeLoaderToOriginal();
	pTheVAOMeshManager->changeLoaderToPly5n();

	pTheVAOMeshManager->SetBasePath("assets/models");

	// Player
	{
		sModelDrawInfo playerBody;
		playerBody.meshFileName = "Player_Body.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerBody, shaderProgramID);

		sModelDrawInfo playerHead;
		playerHead.meshFileName = "Player_Head.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerHead, shaderProgramID);

		sModelDrawInfo playerLeftBottomWing;
		playerLeftBottomWing.meshFileName = "Player_Left_Bottom_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerLeftBottomWing, shaderProgramID);

		sModelDrawInfo playerLeftTopWing;
		playerLeftTopWing.meshFileName = "Player_Left_Top_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerLeftTopWing, shaderProgramID);

		sModelDrawInfo playerRightBottomWing;
		playerRightBottomWing.meshFileName = "Player_Right_Bottom_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerRightBottomWing, shaderProgramID);

		sModelDrawInfo playerRightTopWing;
		playerRightTopWing.meshFileName = "Player_Right_Top_Wing.ply";			// "ssj100_xyz.ply";
		pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(playerRightTopWing, shaderProgramID);
	}

	sModelDrawInfo dalekRes2;
	dalekRes2.meshFileName = "dalek2005_xyz_uv_res_2.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(dalekRes2, shaderProgramID))
	{
		std::cout << "Didn't load the Dalek, res 2" << std::endl;
		std::cout << pTheVAOMeshManager->GetLastError() << std::endl;
	}

	sModelDrawInfo bunnyInfo;
	//bunnyInfo.meshFileName = "bun_res3_xyz.ply";
	//bunnyInfo.meshFileName = "bun_res3_xyz_n_uv.ply";	// NOW, WITH NORMALS!!
	bunnyInfo.meshFileName = "bun_zipper_hi_res_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	if (!pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(bunnyInfo, shaderProgramID))
	{
		std::cout << "Didn't load the bunny" << std::endl;
		std::cout << pTheVAOMeshManager->GetLastError() << std::endl;
	}

	// And the other LODs
		// bun_zipper_res_2_xyz_n_smartUVBlender.ply
	// bun_zipper_res_3_xyz_n_smartUVBlender.ply
	sModelDrawInfo bunnyInfo_Res2;
	bunnyInfo_Res2.meshFileName = "bun_zipper_res_2_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(bunnyInfo_Res2, shaderProgramID);

	sModelDrawInfo bunnyInfo_Res3;
	bunnyInfo_Res3.meshFileName = "bun_zipper_res_3_xyz_n_smartUVBlender.ply";	// NOW, WITH NORMALS!!
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(bunnyInfo_Res3, shaderProgramID);

	sModelDrawInfo gentleTerrain;
	gentleTerrain.meshFileName = "Gentle_Terrain_xyz_uv.ply";			// "ssj100_xyz.ply";
	if (pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(gentleTerrain, shaderProgramID))
	{
		//std::cout << "Gentle terrain is loaded" << std::endl;
	}

	sModelDrawInfo airplaneInfo;
	airplaneInfo.meshFileName = "ssj100_xyz_n_GARBAGE_uv.ply";			// "ssj100_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(airplaneInfo, shaderProgramID);

	sModelDrawInfo backToTheFuture;
	backToTheFuture.meshFileName = "de--lorean_xyz_n_GARBAGE_uv.ply";	// "de--lorean.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(backToTheFuture, shaderProgramID);

	sModelDrawInfo migInfo;
	migInfo.meshFileName = "mig29_xyz_n_GARBAGE_uv.ply";					// "mig29_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(migInfo, shaderProgramID);

	sModelDrawInfo teapotInfo;
	teapotInfo.meshFileName = "Utah_Teapot_xyz_n_GARBAGE_uv.ply";			// "Utah_Teapot_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(teapotInfo, shaderProgramID);

	sModelDrawInfo fishInfo;
	fishInfo.meshFileName = "PacificCod0_rotated_xyz_n_GARBAGE_uv.ply";		// "PacificCod0_rotated_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(fishInfo, shaderProgramID);

	sModelDrawInfo mountainInfo;
	mountainInfo.meshFileName = "mountainterrain2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(mountainInfo, shaderProgramID);

	//sModelDrawInfo metropolisInfo;
	//metropolisInfo.meshFileName = "metropolis.ply";		// "mountainterrain2.ply";
	//pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(metropolisInfo, shaderProgramID);

	sModelDrawInfo threeDModInfo;
	threeDModInfo.meshFileName = "3d-modelC.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(threeDModInfo, shaderProgramID);

	sModelDrawInfo asteroidInfo;
	asteroidInfo.meshFileName = "AsteroidStation.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(asteroidInfo, shaderProgramID);

	sModelDrawInfo frigateInfo;
	frigateInfo.meshFileName = "CyclopsFrigate.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(frigateInfo, shaderProgramID);

	sModelDrawInfo heliosInfo;
	heliosInfo.meshFileName = "HeliosFrigate2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(heliosInfo, shaderProgramID);

	sModelDrawInfo destroyerInfo;
	destroyerInfo.meshFileName = "OxossiDestroyer.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(destroyerInfo, shaderProgramID);

	sModelDrawInfo beaconsInfo;
	beaconsInfo.meshFileName = "beacons.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beaconsInfo, shaderProgramID);

	sModelDrawInfo beacons1Info;
	beacons1Info.meshFileName = "beacons1.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons1Info, shaderProgramID);

	sModelDrawInfo beacons2Info;
	beacons2Info.meshFileName = "beacons2.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons2Info, shaderProgramID);

	sModelDrawInfo beacons3Info;
	beacons3Info.meshFileName = "beacons3.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons3Info, shaderProgramID);

	sModelDrawInfo beacons4Info;
	beacons4Info.meshFileName = "beacons4.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons4Info, shaderProgramID);

	sModelDrawInfo beacons5Info;
	beacons5Info.meshFileName = "beacons5.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons5Info, shaderProgramID);

	sModelDrawInfo beacons6Info;
	beacons6Info.meshFileName = "beacons6.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons6Info, shaderProgramID);

	sModelDrawInfo beacons7Info;
	beacons7Info.meshFileName = "beacons7.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons7Info, shaderProgramID);

	sModelDrawInfo beacons8Info;
	beacons8Info.meshFileName = "beacons8.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons8Info, shaderProgramID);

	sModelDrawInfo beacons9Info;
	beacons9Info.meshFileName = "beacons9.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons9Info, shaderProgramID);

	sModelDrawInfo beacons10Info;
	beacons10Info.meshFileName = "beacons10.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons10Info, shaderProgramID);

	sModelDrawInfo beacons11Info;
	beacons11Info.meshFileName = "beacons11.ply";		// "mountainterrain2.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(beacons11Info, shaderProgramID);

	sModelDrawInfo terrainInfo;
	terrainInfo.meshFileName = "MeshLab_Fractal_Terrain_xyz_n_uv.ply";	// "MeshLab_Fractal_Terrain_xyz.ply";
	// Will alow me to update the vertex data in the mesh
	terrainInfo.bVertexBufferIsDynamic = true;
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(terrainInfo, shaderProgramID);

	sModelDrawInfo cityInfo;
	cityInfo.meshFileName = "ScifiCityRuins.ply";	// "MeshLab_Fractal_Terrain_xyz.ply";
	// Will alow me to update the vertex data in the mesh
	cityInfo.bVertexBufferIsDynamic = true;

	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(cityInfo, shaderProgramID);

	sModelDrawInfo sphereInfo;
	sphereInfo.meshFileName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";			// "Sphere_320_faces_xyz.ply";
	//pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(sphereInfo, shaderProgramID);
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(sphereInfo, shaderProgramID);

	sModelDrawInfo sphereInvertedNormalsInfo;
	sphereInvertedNormalsInfo.meshFileName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(sphereInvertedNormalsInfo, shaderProgramID);

	sModelDrawInfo lonelyTri;
	lonelyTri.meshFileName = "singleTriangle_xyz_n.ply";			// "singleTriangle_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(lonelyTri, shaderProgramID);

	sModelDrawInfo cube1x1x1;
	cube1x1x1.meshFileName = "cube_flat_shaded_xyz_n_uv.ply";			// "cube_flat_shaded_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(cube1x1x1, shaderProgramID);


	//sModelDrawInfo oldhouse;
	//oldhouse.meshFileName = "Old_House_Combined_xyz_n.ply";		
	//pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(oldhouse, shaderProgramID);

	sModelDrawInfo skyPirate;
	skyPirate.meshFileName = "Sky_Pirate_Combined_xyz_n_BlenderSmart_uv.ply";
	if (pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(skyPirate, shaderProgramID))
	{
		//std::cout << "Sky Pirate is loaded" << std::endl;
	}

	// At this point, mesh in in GPU
	//std::cout << "Mesh was loaded OK" << std::endl;


	sModelDrawInfo imposterObject_1;
	imposterObject_1.meshFileName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";				// Flat, 2 sided 'classic' imposter
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(imposterObject_1, shaderProgramID);

	sModelDrawInfo imposterObject_2;
	imposterObject_2.meshFileName = "1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply";				// More complicated "star" shaped imposter
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(imposterObject_2, shaderProgramID);

	sModelDrawInfo imposterObject_3;
	imposterObject_3.meshFileName = "Sphere_UV_xyz_n_uv.ply";							// A simple UV mapped sphere (low poly)
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(imposterObject_3, shaderProgramID);

	//	if ( ! ::g_pTheTextureManager->Create2DTextureFromBMPFile("Justin.bmp", true ) )
	//	{
	//		std::cout << "Didn't load texture" << std::endl;
	//	}
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("grass.bmp", true );
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("water-1514818_960_720.bmp", true );
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("brick-wall.bmp", true );
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("220px-Emma_Watson_2013.bmp", true );
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Flowers.bmp", true );

		// For the particle imposters
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Smoke_1.bmp", true );
	//	::g_pTheTextureManager->Create2DTextureFromBMPFile("Plasma_Ring.bmp", true );
	//
	//	if ( ::g_pTheTextureManager->Create2DTextureFromBMPFile("rock_cave_stylized_height.bmp", true ) )
	//	{
	//		std::cout << "loaded height map, too" << std::endl;
	//	}




	sModelDrawInfo testObject;
	testObject.meshFileName = "SM_Env_Tree_Canopy_04.ply";							// A simple UV mapped sphere (low poly)
	pTheVAOMeshManager->SetBasePath("assets/models/final");
	pTheVAOMeshManager->LoadModelIntoVAO_ASYNC(testObject, shaderProgramID);

	pTheVAOMeshManager->SetBasePath("assets/models");
	pTheVAOMeshManager->m_LoadModelFromFile_ASYNC();
	return;
}


void LoadPlayerModels_LOJAM( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw )
{
	{ // STARTOF: Player
		cMeshSceneObject* pPlayerBody = new cMeshSceneObject();
		cMeshSceneObject* pPlayerHead = new cMeshSceneObject();
		cMeshSceneObject* pPlayerTopLeftWing = new cMeshSceneObject();
		cMeshSceneObject* pPlayerTopRightWing = new cMeshSceneObject();
		cMeshSceneObject* pPlayerBottomLeftWing = new cMeshSceneObject();
		cMeshSceneObject* pPlayerBottomRightWing = new cMeshSceneObject();

		sTextureInfo playerObjectTexture;
		playerObjectTexture.name = "water-1514818_960_720.bmp";
		playerObjectTexture.strength = 1.0f;

		{//Player body
			pPlayerBody->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerBody->setAlphaTransparency( 1.0f );
			pPlayerBody->friendlyName = "PlayerBody";
			pPlayerBody->position = glm::vec3(0.0f, 0.0f, 15.0f);
			pPlayerBody->uniformScale = 1.0f;
//			pPlayerBody->meshName = "Player_Body.ply";	
			pPlayerBody->vecLODMeshs.push_back(sLODInfo("Player_Body.ply"));

//			pTestObject->bIsWireFrame = true;
//			pPlayerBody->bDontLight = true;

			pPlayerBody->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->uniformScale = 1.0f;
			vec_pObjectsToDraw.push_back( pPlayerBody );
		}//Player body

		{//Player head
			pPlayerHead->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerHead->setAlphaTransparency( 1.0f );
			pPlayerHead->friendlyName = "PlayerHead";
			pPlayerHead->position = glm::vec3(0.0f, 0.0f, 0.0f);
			pPlayerHead->uniformScale = 1.0f;
			//pPlayerHead->meshName = "Player_Head.ply";	
			pPlayerHead->vecLODMeshs.push_back(sLODInfo("Player_Head.ply"));
			pPlayerHead->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->vec_pChildObjectsToDraw.push_back(pPlayerHead);
		}//Player body

		{//Player Top Left Wing
			pPlayerTopLeftWing->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerTopLeftWing->setAlphaTransparency( 1.0f );
			pPlayerTopLeftWing->friendlyName = "PlayerTopLeftWing";
			pPlayerTopLeftWing->position = glm::vec3(0.0f, 0.0f, 0.0f);
			pPlayerTopLeftWing->uniformScale = 1.0f;
//			pPlayerTopLeftWing->meshName = "Player_Left_Top_Wing.ply";	
			pPlayerTopLeftWing->vecLODMeshs.push_back(sLODInfo("Player_Left_Top_Wing.ply"));
			pPlayerTopLeftWing->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->vec_pChildObjectsToDraw.push_back(pPlayerTopLeftWing);
		}//Player Top Left Wing

		{//Player Top Right Wing
			pPlayerTopRightWing->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerTopRightWing->setAlphaTransparency( 1.0f );
			pPlayerTopRightWing->friendlyName = "PlayerTopRightWing";
			pPlayerTopRightWing->position = glm::vec3(0.0f, 0.0f, 0.0f);
			pPlayerTopRightWing->uniformScale = 1.0f;
//			pPlayerTopRightWing->meshName = "Player_Right_Top_Wing.ply";	
			pPlayerTopRightWing->vecLODMeshs.push_back(sLODInfo("Player_Right_Top_Wing.ply"));
			pPlayerTopRightWing->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->vec_pChildObjectsToDraw.push_back(pPlayerTopRightWing);
		}//Player Top Right Wing
	
		{//Player Bottom Right Wing
			pPlayerBottomRightWing->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerBottomRightWing->setAlphaTransparency( 1.0f );
			pPlayerBottomRightWing->friendlyName = "PlayerBottomRightWing";
			pPlayerBottomRightWing->position = glm::vec3(0.0f, 0.0f, 0.0f);
			pPlayerBottomRightWing->uniformScale = 1.0f;
//			pPlayerBottomRightWing->meshName = "Player_Right_Bottom_Wing.ply";	
			pPlayerBottomRightWing->vecLODMeshs.push_back(sLODInfo("Player_Right_Bottom_Wing.ply"));
			pPlayerBottomRightWing->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->vec_pChildObjectsToDraw.push_back(pPlayerBottomRightWing);
		}//Player Bottom Right Wing
	
		{//Player Bottom Left Wing
			pPlayerBottomLeftWing->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
			pPlayerBottomLeftWing->setAlphaTransparency( 1.0f );
			pPlayerBottomLeftWing->friendlyName = "PlayerBottomLeftWing";
			pPlayerBottomLeftWing->position = glm::vec3(0.0f, 0.0f, 0.0f);
			pPlayerBottomLeftWing->uniformScale = 1.0f;
//			pPlayerBottomLeftWing->meshName = "Player_Left_Bottom_Wing.ply";	
			pPlayerBottomLeftWing->vecLODMeshs.push_back(sLODInfo("Player_Left_Bottom_Wing.ply"));
			pPlayerBottomLeftWing->vecTextures.push_back( playerObjectTexture );
			pPlayerBody->vec_pChildObjectsToDraw.push_back(pPlayerBottomLeftWing);
		}//Player Bottom Left Wing
	
	}// ENDOF: Player

	return;
}

void LoadABunchOfLODBunnies( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw )
{
	// Load a whole lot of bunny rabbits...

	// bun_zipper_hi_res_xyz_n_smartUVBlender.ply
	// bun_zipper_res_2_xyz_n_smartUVBlender.ply
	// bun_zipper_res_3_xyz_n_smartUVBlender.ply

	//float gridSize = 10.0f;
	//float gridWidth = 250.0f; 

	float gridSize = 50.0f;
	float gridWidth = 100.0f; 

	const float BUNNY_Y_OFFSET = -10.0f;

	for ( float x = -gridWidth; x <= (gridWidth + gridSize); x += gridSize )
	{
		for ( float z = -gridWidth; z <= (gridWidth+ gridSize); z += gridSize )
		{
			cMeshSceneObject* pBunny = new cMeshSceneObject();
			pBunny->position = glm::vec3( x, BUNNY_Y_OFFSET, z );
			pBunny->setDiffuseColour( glm::vec3( 0.0f, 0.0f, 0.0f ) );

			// Next from there to infinity
			pBunny->vecLODMeshs.push_back(sLODInfo("bun_zipper_res_3_xyz_n_smartUVBlender.ply", FLT_MAX) );		
//			pBunny->vecLODMeshs.push_back(sLODInfo("PacificCod0_rotated_xyz_n_GARBAGE_uv.ply", FLT_MAX) );		
			// next one at less than 50 units from camera (up to 10)
			pBunny->vecLODMeshs.push_back(sLODInfo("bun_zipper_res_2_xyz_n_smartUVBlender.ply", 100.0f) );		
			// High resolution at less than 10 units from camera
			pBunny->vecLODMeshs.push_back(sLODInfo("bun_zipper_hi_res_xyz_n_smartUVBlender.ply", 50.0f) );	


			pBunny->friendlyName = "Bunny";
//			pBunny->setUniformScale( 50.0f );
			pBunny->uniformScale = 100.0f;
			sTextureInfo grassTexture;
			grassTexture.name = "grass.bmp";
			grassTexture.strength = 1.0f;
			pBunny->vecTextures.push_back( grassTexture );

			pBunny->pDebugRenderer = ::g_pDebugRenderer;

			pBunny->bIsVisible = true;
			//pBunny->bIsWireFrame = true;

			vec_pObjectsToDraw.push_back( pBunny );
		}	// z
	}	// x


	return;
}



	
// Loads the models we are drawing into the vector
void LoadModelsIntoScene( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw )
{
	// for LOD example
//	LoadABunchOfLODBunnies( vec_pObjectsToDraw );


	LoadPlayerModels_LOJAM(vec_pObjectsToDraw);


// *********************************************************************************************
	{	// Player 1, which uses a loaded skinned mesh
		cMeshSceneObject* pPlayer_01 = new cMeshSceneObject();
		pPlayer_01->position = glm::vec3( 0.0f, 0.0f, 0.0f );
		pPlayer_01->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 0.0f ) );		// Yellow
		pPlayer_01->vecLODMeshs.push_back(sLODInfo("RPG-Character.fbx"));		// "Utah_Teapot_xyz.ply";
		pPlayer_01->friendlyName = "Player1";
		pPlayer_01->uniformScale = 0.01f;

//		pPlayer_01->setMeshOrientationEulerAngles( glm::vec3( 0.0f, 90.0f, 0.0f ), true );

		pPlayer_01->pDebugRenderer = ::g_pDebugRenderer;

		pPlayer_01->bIsVisible = true;

		pPlayer_01->pSimpleSkinnedMesh = ::g_pTheVAOMeshManager->FindSkinnedMeshModelByName( "RPG-Character" );

		pPlayer_01->pAniState = new cAnimationState();
		pPlayer_01->pAniState->defaultAnimation.name = "Idle";

		vec_pObjectsToDraw.push_back( pPlayer_01 );
	}
// *********************************************************************************************

	{	// This will be our "skybox" object.
		// (could also be a cube, or whatever)
		cMeshSceneObject* pSkyBoxObject = new cMeshSceneObject();
		pSkyBoxObject->setDiffuseColour( glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f ) );
		pSkyBoxObject->bUseVertexColour = false;
		pSkyBoxObject->friendlyName = "SkyBoxObject";
		pSkyBoxObject->uniformScale = 5000.0f;
//		pSkyBoxObject->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
		pSkyBoxObject->vecLODMeshs.push_back(sLODInfo("Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply"));
//		pSkyBoxObject->bIsWireFrame = true;
		sTextureInfo skyboxTextureInfo;
		skyboxTextureInfo.name = "CityCubeMap";
		skyboxTextureInfo.strength = 1.0f;
		pSkyBoxObject->vecTextures.push_back( skyboxTextureInfo );

		// Invisible until I need to draw it
		pSkyBoxObject->bIsVisible = false;

		vec_pObjectsToDraw.push_back( pSkyBoxObject );
	}


	{
		cMeshSceneObject* pDalek = new cMeshSceneObject();
		pDalek->position = glm::vec3( 0.0f, 0.0f, 0.0f );
		pDalek->setMeshOrientationEulerAngles( glm::vec3( -90.0f, 0.0f, 0.0f), true );
//		pDalek->setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f), true );
		pDalek->setDiffuseColour( glm::vec3( 0.0f, 0.0f, 0.0f ) );
//		pDalek->meshName = "dalek2005_xyz_uv_res_2.ply";		// "Utah_Teapot_xyz.ply";
		pDalek->vecLODMeshs.push_back(sLODInfo("dalek2005_xyz_uv_res_2.ply"));		// "Utah_Teapot_xyz.ply";
		pDalek->friendlyName = "Dalek";
		pDalek->uniformScale = 0.5f;
		sTextureInfo grassTexture;
		grassTexture.name = "grass.bmp";
		grassTexture.strength = 1.0f;
		pDalek->vecTextures.push_back( grassTexture );

		pDalek->pDebugRenderer = ::g_pDebugRenderer;

		pDalek->bIsVisible = false;

		vec_pObjectsToDraw.push_back( pDalek );
	}	
	
	//std::vector< cMeshObject* > vec_pObjectsToDraw;
	{
		cMeshSceneObject* pTeapot = new cMeshSceneObject();
		pTeapot->position = glm::vec3( -40.0f, 0.0f, 0.0f );
		pTeapot->setMeshOrientationEulerAngles( glm::vec3( -90.0f, 0.0f, 0.0f), true );
//		pTeapot->objColour = glm::vec3( 1.0f, 0.0f, 0.0f );
		pTeapot->setDiffuseColour( glm::vec3( 0.0f, 0.0f, 0.0f ) );

		// 50% transparent
		// (from 0 to 1, where 0 is invisible)
		pTeapot->setAlphaTransparency( 0.5f );

		// Make the terrain really shinny...
		pTeapot->setSpecularPower( 100.0f );

//		pTeapot->meshName = "Utah_Teapot_xyz_n_GARBAGE_uv.ply";		// "Utah_Teapot_xyz.ply";
		pTeapot->vecLODMeshs.push_back(sLODInfo("Utah_Teapot_xyz_n_GARBAGE_uv.ply"));
																	// Note scale...
//		pTeapot->nonUniformScale = glm::vec3(0.005f,0.005f,0.005f);
		pTeapot->uniformScale = 0.5f;
		// Set the debug renderer here

		// Texture on the teapot
		sTextureInfo grassTexture;
		grassTexture.name = "grass.bmp";
		grassTexture.strength = 1.0f;
		pTeapot->vecTextures.push_back( grassTexture );

		pTeapot->pDebugRenderer = ::g_pDebugRenderer;

		//vec_pObjectsToDraw.push_back( pTeapot );
	}

	{
		cMeshSceneObject* pCod = new cMeshSceneObject();
		pCod->position = glm::vec3( +3.0f, 0.0f, 0.0f );
//		pCod->preRotation = glm::vec3( glm::radians(90.0f), 0.0f, 0.0f );

		pCod->setMeshOrientationEulerAngles( glm::vec3( glm::radians(90.0f), 0.0f, 0.0f ), false );

//		pCod->objColour = glm::vec3( 0.0f, 1.0f, 0.0f );
		pCod->setDiffuseColour( glm::vec3(0.0f, 1.0f, 0.0f) );
//		pCod->meshName = "PacificCod0_rotated_xyz_n_GARBAGE_uv.ply";		// "PacificCod0_rotated_xyz.ply";
		pCod->vecLODMeshs.push_back(sLODInfo("PacificCod0_rotated_xyz_n_GARBAGE_uv.ply"));
		pCod->uniformScale = 10.0f;
			
		pCod->pDebugRenderer = ::g_pDebugRenderer;

		sTextureInfo justin;
		justin.name = "Justin.bmp";
		justin.strength = 1.0f;
		pCod->vecTextures.push_back( justin );



		// Note scale...
		//vec_pObjectsToDraw.push_back( pCod );
	}

	{
		cMeshSceneObject* pCar = new cMeshSceneObject();
		pCar->position = glm::vec3( 0.0f, 20.0f, -20.0f );
//		pCar->objColour = glm::vec3( 0.0f, 0.0f, 1.0f );
		pCar->setDiffuseColour( glm::vec3(0.0f, 0.0f, 1.0f) );
		//
		pCar->setAlphaTransparency( 0.2f );
		// 
//		pCar->meshName = "de--lorean_xyz_n_GARBAGE_uv.ply";		// "de--lorean.ply";
		pCar->vecLODMeshs.push_back(sLODInfo("de--lorean_xyz_n_GARBAGE_uv.ply"));
		pCar->uniformScale = 0.5f;
		pCar->pDebugRenderer = ::g_pDebugRenderer;

		sTextureInfo flowers;
		flowers.name = "Flowers.bmp";
		flowers.strength = 1.0f;
		pCar->vecTextures.push_back( flowers );

		sTextureInfo justin;
		justin.name = "Justin.bmp";
		justin.strength = 0.0f;
		pCar->vecTextures.push_back( justin );


		//vec_pObjectsToDraw.push_back( pCar );
	}

	{
		cMeshSceneObject* pMig = new cMeshSceneObject();
		pMig->friendlyName = "Mig-29";		// 

		pMig->position = glm::vec3( 0.0f, -3.0f, 0.0f );
//		pMig->objColour = glm::vec3( 0.1f, 0.5f, 0.31f );
		pMig->setDiffuseColour( glm::vec3(0.1f, 0.5f, 0.31f) );
		// SCALE is 1.0f!! Makes physics simpler
		pMig->uniformScale = 1.0f;
//		pMig->nonUniformScale = glm::vec3(9.0f, 9.0f, 9.0f);
//		pMig->meshName = "mig29_xyz_n_GARBAGE_uv.ply";			// "mig29_xyz.ply";
		pMig->vecLODMeshs.push_back(sLODInfo("mig29_xyz_n_GARBAGE_uv.ply"));
		//pMig->bIsWireFrame = true;
		pMig->pDebugRenderer = ::g_pDebugRenderer;
		// Note scale...
		//vec_pObjectsToDraw.push_back( pMig );
	}

	//cMeshObject* pRogerRabbit = NULL;

	{
		//cMeshObject* pBunny = new cMeshObject();
		::g_pRogerRabbit = new cMeshSceneObject();
//		::g_pRogerRabbit->preRotation = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_pRogerRabbit->position = glm::vec3(  0.0f, 25.0f, 0.0f );
		//::g_pRogerRabbit->postRotation = glm::vec3( glm::radians(45.0f), 0.0f, 0.0f );

		::g_pRogerRabbit->setMeshOrientationEulerAngles( glm::vec3( glm::radians(45.0f), 0.0f, 0.0f ) );

//		::g_pRogerRabbit->nonUniformScale = glm::vec3(9.0f, 9.0f, 9.0f);
		::g_pRogerRabbit->uniformScale = 5.0f;
//		::g_pRogerRabbit->objColour = glm::vec3( 1.0f, 1.0f, 0.0f );
//		::g_pRogerRabbit->setDiffuseColour( glm::vec3(1.0f, 1.0f, 0.0f) );
		::g_pRogerRabbit->bUseVertexColour = true;

		// Make the terrain really shinny...
		::g_pRogerRabbit->setSpecularPower( 10.0f );
//		::g_pRogerRabbit->setSpecularColour( glm::vec3(1.0f, 0.0f, 0.0f) );
//		::g_pRogerRabbit->setSpecularPower( 0.54f );


		// "Gold" bunny?
		::g_pRogerRabbit->setDiffuseColour( glm::vec3(1.0f, 223.0f/255.0f, 0.0f) );
		::g_pRogerRabbit->bUseVertexColour = false;
		// Taken from: https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		// Note that the specular HIGHLIGHT colour is DIFFERENT from 
		// the diffuse... 
		::g_pRogerRabbit->setSpecularPower( 100.0f );
		::g_pRogerRabbit->setSpecularColour( glm::vec3(1.000f, 0.766f, 0.336f) );

		

//		::g_pRogerRabbit->meshName = "bun_res3_xyz.ply";
//		::g_pRogerRabbit->meshName = "bun_zipper_hi_res_xyz_n_smartUVBlender.ply";
		g_pRogerRabbit->vecLODMeshs.push_back(sLODInfo("bun_zipper_hi_res_xyz_n_smartUVBlender.ply"));
		::g_pRogerRabbit->bIsVisible = true;
		// 
		::g_pRogerRabbit->friendlyName = "Roger";

		::g_pRogerRabbit->pDebugRenderer = ::g_pDebugRenderer;


		sTextureInfo flowers;
		flowers.name = "Flowers.bmp";
		flowers.strength = 1.0f;
		::g_pRogerRabbit->vecTextures.push_back( flowers );

		sTextureInfo justin;
		justin.name = "Justin.bmp";
		justin.strength = 0.0f;
		::g_pRogerRabbit->vecTextures.push_back( justin );


		//vec_pObjectsToDraw.push_back( ::g_pRogerRabbit );
	}
	int indexSelectedObject = 0;
	{
		cMeshSceneObject* pBunny = new cMeshSceneObject();
//		pBunny->preRotation = glm::vec3( 0.0f, 0.0f, 0.0f );

		pBunny->setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );

		pBunny->position = glm::vec3(  0.0f, 25.0f, 0.0f );
//		pBunny->postRotation = glm::vec3( glm::radians(45.0f), 0.0f, 0.0f );
		pBunny->uniformScale = 9.0f;
//		pBunny->objColour = glm::vec3( 1.0f, 1.0f, 0.0f );
		pBunny->setDiffuseColour( glm::vec3(1.0f, 1.0f, 0.0f) );
//		pBunny->meshName = "bun_zipper_hi_res_xyz_n_smartUVBlender.ply";
		pBunny->vecLODMeshs.push_back(sLODInfo("bun_zipper_hi_res_xyz_n_smartUVBlender.ply"));
		pBunny->bIsVisible = true;
		pBunny->friendlyName = "Bugs";

		pBunny->pDebugRenderer = ::g_pDebugRenderer;

		//vec_pObjectsToDraw.push_back( pBunny );
	}


	{
		cMeshSceneObject* pTerrain = new cMeshSceneObject();
		//pTerrain->position = glm::vec3(  0.0f, -10.0f, 0.0f );
		pTerrain->position = glm::vec3(  0.0f, 100.0f, 0.0f );
//		pTerrain->objColour = glm::vec3( 1.0f, 1.0f, 1.0f );
//		pTerrain->setDiffuseColour( glm::vec3(1.0f, 1.0f, 1.0f) );
		pTerrain->setDiffuseColour( glm::vec3(0.0f, 0.0f, 0.0f) );
		
		// Make the terrain really shinny...
		pTerrain->setSpecularColour( glm::vec3(1.0f,1.0f,1.0f) );
		pTerrain->setSpecularPower( 100.0f );
//		pTerrain->setSpecularPower( 2.0f );


		pTerrain->bUseVertexColour = false;
//		pTerrain->u
//		pTerrain->meshName = "MeshLab_Fractal_Terrain_xyz_n_uv.ply";		// "MeshLab_Fractal_Terrain_xyz.ply";
		//pTerrain->vecLODMeshs.push_back(sLODInfo("MeshLab_Fractal_Terrain_xyz_n_uv.ply"));
		pTerrain->vecLODMeshs.push_back(sLODInfo("ScifiCityRuins.ply"));
		pTerrain->friendlyName = "The Terrain";
		//pTerrain->bIsWireFrame = true;
		pTerrain->bIsVisible = true;

		pTerrain->pDebugRenderer = ::g_pDebugRenderer;



		sTextureInfo bricks;
		bricks.name = "brick-wall.bmp";
		bricks.strength = 0.5f;
		pTerrain->vecTextures.push_back( bricks );

		sTextureInfo justin;
		justin.name = "Justin.bmp";
		justin.strength = 0.5f;
		pTerrain->vecTextures.push_back( justin );

		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		//vec_pObjectsToDraw.push_back( pTerrain );
	}

//	// Updated physics object
//	{	// This sphere is the tiny little debug sphere
//		cMeshSceneObject* pPlayerSphere = new cMeshSceneObject();
////		pPlayerSphere->objColour = glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f );
//		pPlayerSphere->setDiffuseColour( glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f ) );
//		pPlayerSphere->friendlyName = "Player";
//		float scale = 1.0f;
//		pPlayerSphere->nonUniformScale = glm::vec3(scale,scale,scale);
//		pPlayerSphere->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";			// "Sphere_320_faces_xyz.ply";
//		pPlayerSphere->bIsWireFrame = true;
//
//		pPlayerSphere->position = glm::vec3( -100.0f, 10.0f, 0.0f );
//		pPlayerSphere->velocity = glm::vec3( 5.0f, 10.0f, 0.0f );		// starts at "up" at 10 mps
//		pPlayerSphere->accel = glm::vec3( 0.0f, -5.0f, 0.0f );
//		pPlayerSphere->bIsUpdatedByPhysics = true;
//
//		pPlayerSphere->pDebugRenderer = ::g_pDebugRenderer;
//
//		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
//		vec_pObjectsToDraw.push_back( pPlayerSphere );
//	}


	const unsigned int NUMBER_OF_BALLS = 100;

	for ( unsigned int count = 0; count != NUMBER_OF_BALLS; count++ )
	// Updated physics object
	{	// This sphere is the tiny little debug sphere
		cMeshSceneObject* pBouncyBall = new cMeshSceneObject();
//		pBouncyBall->objColour = glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f );
		pBouncyBall->setDiffuseColour( glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f ) );
		pBouncyBall->uniformScale = 5.0f;
//		pBouncyBall->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";		// "Sphere_320_faces_xyz.ply";
		pBouncyBall->vecLODMeshs.push_back(sLODInfo("Sphere_320_faces_xyz_n_GARBAGE_uv.ply"));
		pBouncyBall->bIsWireFrame = true;

		pBouncyBall->position = glm::vec3( getRandBetween0and1<float>() * 199.0f - 99.0f, 
		                                   getRandBetween0and1<float>() * 199.0f + 0.5f,
		                                   getRandBetween0and1<float>() * 199.0f - 99.0f);
		pBouncyBall->velocity = glm::vec3( getRandBetween0and1<float>() * 100.0f - 5.0f,
		                                   getRandBetween0and1<float>() * 100.0f - 5.0f, 
		                                   getRandBetween0and1<float>() * 100.0f - 5.0f );	
//
//		pBouncyBall->accel = glm::vec3( 0.0f, -5.0f, 0.0f );
		pBouncyBall->accel = g_ACCEL_GRAVITY;

		pBouncyBall->bIsUpdatedByPhysics = true;

//		pBouncyBall->radius = 5.0f;	// Be careful

		// Set the type
		pBouncyBall->pTheShape = new sSphere(5.0f);
		pBouncyBall->shapeType = cMeshSceneObject::SPHERE;

		pBouncyBall->pDebugRenderer = ::g_pDebugRenderer;


		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		//vec_pObjectsToDraw.push_back( pBouncyBall );
	}


	// ENDOF: Updated physics object

	{	// This sphere is the tiny little debug sphere
		cMeshSceneObject* pDebugSphere = new cMeshSceneObject();
		pDebugSphere->position = glm::vec3( 0.0f, 0.0f, 0.0f );
//		pDebugSphere->objColour = glm::vec3( 0.0f, 1.0f, 0.0f );
		pDebugSphere->setDiffuseColour( glm::vec3(0.0f, 1.0f, 0.0f) );
		pDebugSphere->friendlyName = "DebugSphere";
		pDebugSphere->uniformScale = 0.1f;
//		pDebugSphere->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";		// "Sphere_320_faces_xyz.ply";
		pDebugSphere->vecLODMeshs.push_back(sLODInfo("Sphere_320_faces_xyz_n_GARBAGE_uv.ply"));
		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = false;
		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;

		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		vec_pObjectsToDraw.push_back( pDebugSphere );
	}


	{	// This sphere is the tiny little debug sphere
		cMeshSceneObject* pTriangle = new cMeshSceneObject();
//		pTriangle->objColour = glm::vec3( 0.0f, 1.0f, 1.0f );
		pTriangle->setDiffuseColour( glm::vec3( 0.0f, 1.0f, 1.0f ) );
		pTriangle->friendlyName = "SingleTriangle";
//		pTriangle->meshName = "singleTriangle_xyz_n.ply";		// "singleTriangle_xyz.ply";
		pTriangle->vecLODMeshs.push_back(sLODInfo("singleTriangle_xyz_n.ply"));
		pTriangle->bDontLight = true;
		pTriangle->bIsWireFrame = true;

		pTriangle->position = glm::vec3(  -10.0f, 15.0f, 0.0f );
		//pTriangle->preRotation = glm::vec3( 0.0f, 
		//									glm::radians(15.0f), 
		//									glm::radians(75.0f));
		pTriangle->setMeshOrientationEulerAngles( glm::vec3( 0.0f, 15.0f, 75.0f), true );

//		pTriangle->setUniformScale( 1.0f );

		sTriangle* pTri = new sTriangle();
		// This is the "phsyics object" for this triangle
		// This is "model space"
		pTri->v[0] = glm::vec3(-30.0f, 0.0f, -30.0f);
		pTri->v[1] = glm::vec3( 30.0f, 0.0f, -30.0f);
		pTri->v[2] = glm::vec3(  0.0f, 0.0f,  30.0f);

		pTriangle->pTheShape = pTri;
		pTriangle->shapeType = cMeshSceneObject::TRIANGLE;
		
		pTriangle->pDebugRenderer = ::g_pDebugRenderer;

		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		//vec_pObjectsToDraw.push_back( pTriangle );
	}

	{	// This sphere is the tiny little debug sphere
		cMeshSceneObject* pCubeForBallsToBounceIn = new cMeshSceneObject();
//		pCubeForBallsToBounceIn->objColour = glm::vec3( 0.0f, 1.0f, 0.0f );
		pCubeForBallsToBounceIn->setDiffuseColour( glm::vec3( 0.0f, 1.0f, 0.0f ) );
		pCubeForBallsToBounceIn->bDontLight = true;
		pCubeForBallsToBounceIn->friendlyName = "CubeBallsBounceIn";
//		pCubeForBallsToBounceIn->meshName = "cube_flat_shaded_xyz_n_uv.ply";		// "cube_flat_shaded_xyz.ply";
		pCubeForBallsToBounceIn->vecLODMeshs.push_back(sLODInfo("cube_flat_shaded_xyz_n_uv.ply"));
		pCubeForBallsToBounceIn->uniformScale = 100.0f;
		// Cube is 2x2x2, so with a scale of 100x means it's
		//	200x200x200, centred around the origin (0,0,0)
		// The GROUND_PLANE_Y = -3.0f, so place it +97.0 lines up the 'bottom'
		pCubeForBallsToBounceIn->position = glm::vec3(  0.0f, 97.0f, 0.0f );
		pCubeForBallsToBounceIn->bIsWireFrame = true;
				
		pCubeForBallsToBounceIn->pDebugRenderer = ::g_pDebugRenderer;

		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		//vec_pObjectsToDraw.push_back( pCubeForBallsToBounceIn );
	}


	{	//
		cMeshSceneObject* pPirate = new cMeshSceneObject();
		pPirate->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
//		pPirate->setAlphaTransparency( 0.50f );
		pPirate->setAlphaTransparency( 1.0f );
		//plighthouse->friendlyName = "Lighthouse";
		//plighthouse->meshName = "Lighthouse_Combined_xyz_n.ply";		// "cube_flat_shaded_xyz.ply";
		//plighthouse->friendlyName = "OldHouse";
		//plighthouse->meshName = "Old_House_Combined_xyz_n.ply";		// "cube_flat_shaded_xyz.ply";
		pPirate->friendlyName = "SkyPirate";
//		pPirate->meshName = "Sky_Pirate_Combined_xyz_n_BlenderSmart_uv.ply";		// "cube_flat_shaded_xyz.ply";
		pPirate->vecLODMeshs.push_back(sLODInfo("Sky_Pirate_Combined_xyz_n_BlenderSmart_uv.ply"));
		pPirate->uniformScale = 1.0f;
		pPirate->position = glm::vec3( 15.0f, 10.0f, 0.0f );
//		plighthouse->bIsWireFrame = true;
		//vec_pObjectsToDraw.push_back( pPirate );
	}


//    ___   ___ _    _        _    ___               _    ____            ___   __  __                       __  
//   |_  ) / __(_)__| |___ __| |  / _ \ _  _ __ _ __| |  / / _|___ _ _   / _ \ / _|/ _|___ __ _ _ ___ ___ _ _\ \ 
//    / /  \__ \ / _` / -_) _` | | (_) | || / _` / _` | | |  _/ _ \ '_| | (_) |  _|  _(_-</ _| '_/ -_) -_) ' \| |
//   /___| |___/_\__,_\___\__,_|  \__\_\\_,_\__,_\__,_| | |_| \___/_|    \___/|_| |_| /__/\__|_| \___\___|_||_| |
//                                                       \_\                                                 /_/ 
	{	// One of the "star shaped" imposter object
		cMeshSceneObject* p2SidedQuad = new cMeshSceneObject();
		p2SidedQuad->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
		p2SidedQuad->setAlphaTransparency( 1.0f );
		p2SidedQuad->friendlyName = "2SidedQuad";
//		p2SidedQuad->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";		// Multi-faced imposter
		p2SidedQuad->vecLODMeshs.push_back(sLODInfo("1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply"));

		p2SidedQuad->uniformScale = 10.0f;
		p2SidedQuad->position = glm::vec3(0.0f, 0.0f, 0.0f);
		p2SidedQuad->setMeshOrientationEulerAngles( glm::vec3(0.0f, 0.0f, 0.0f) );
		p2SidedQuad->bIsVisible = false;
		vec_pObjectsToDraw.push_back( p2SidedQuad );
	}		
	
	
	{	// One of the "star shaped" imposter object
		cMeshSceneObject* pSmoke = new cMeshSceneObject();
		pSmoke->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
		pSmoke->setAlphaTransparency( 1.0f );
		pSmoke->friendlyName = "SmokeObjectStar";
		// ***********************************************************
		// STAR SHAPED IMPOSTER
		// ***********************************************************
//		pSmoke->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";		// Flat, classic imposter
//		pSmoke->meshName = "1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply";		// Multi-faced imposter
		pSmoke->vecLODMeshs.push_back(sLODInfo("1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply"));
//		pSmoke->meshName = "Sphere_UV_xyz_n_uv.ply";					// Imposter can be any shape, really

		sTextureInfo smokeTex;
		smokeTex.name = "Smoke_1.bmp";
		smokeTex.strength = 1.0f;

		pSmoke->vecTextures.push_back( sTextureInfo(smokeTex) );
		pSmoke->uniformScale = 1.0f;
		pSmoke->bIsVisible = false;
		vec_pObjectsToDraw.push_back( pSmoke );
	}	
	


	{	// The 2D 2 triangle "flat" imposter object
		cMeshSceneObject* pSmoke = new cMeshSceneObject();
		pSmoke->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
		pSmoke->setAlphaTransparency( 1.0f );
		pSmoke->friendlyName = "SmokeObjectQuad";
		// ***********************************************************
		// FLAT IMPOSTER
		// ***********************************************************
//		pSmoke->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";		// Flat, classic imposter
		pSmoke->vecLODMeshs.push_back(sLODInfo("1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply"));
//		pSmoke->meshName = "1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply";		// Multi-faced imposter
//		pSmoke->meshName = "Sphere_UV_xyz_n_uv.ply";					// Imposter can be any shape, really

		// NOTE: It's the same texture map, though...
		sTextureInfo smokeTex;
		smokeTex.name = "Smoke_1.bmp";
		smokeTex.strength = 1.0f;

		pSmoke->vecTextures.push_back( sTextureInfo(smokeTex) );
		pSmoke->uniformScale = 1.0f;
		pSmoke->bIsVisible = false;
		vec_pObjectsToDraw.push_back( pSmoke );
	}	


	{	// The 2D 2 triangle "flat" imposter object
		cMeshSceneObject* pPlasmaBlast = new cMeshSceneObject();
		pPlasmaBlast->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
		pPlasmaBlast->setAlphaTransparency( 1.0f );
		pPlasmaBlast->friendlyName = "PlasmaRingImposterObject";
		// ***********************************************************
		// FLAT IMPOSTER
		// ***********************************************************
//		pPlasmaBlast->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";		// Flat, classic imposter
		pPlasmaBlast->vecLODMeshs.push_back(sLODInfo("1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply"));
//		pSmoke->meshName = "1x1_Star_1_Quad_2_Sided_xyz_n_uv.ply";		// Multi-faced imposter
//		pSmoke->meshName = "Sphere_UV_xyz_n_uv.ply";					// Imposter can be any shape, really

		// NOTE: This is a "plasma ring" (that's what I googled...)
		sTextureInfo plasmaRingTexture;
		plasmaRingTexture.name = "Plasma_Ring.bmp";
		plasmaRingTexture.strength = 1.0f;

		pPlasmaBlast->vecTextures.push_back( sTextureInfo(plasmaRingTexture) );
		pPlasmaBlast->uniformScale = 1.0f;
		pPlasmaBlast->bIsVisible = false;
		vec_pObjectsToDraw.push_back( pPlasmaBlast );
	}	
	

	
	return;
}



// For the particle emitter examples
void setUpParticleEmitters(void)
{

	{	//STARTOF: Star shaped smoke emitter
		cParticleEmitter* pPE_Smoke_01 = new cParticleEmitter();

		// This takes a while
		cParticleEmitter::sParticleCreationParams particleParams;
		particleParams.totalNumberOfParticles = 500;	// Max # of particles ALIVE at ONE TIME
		particleParams.minLifeTime = 5.0f;
		particleParams.maxLifeTime = 10.0f;
		// Will fade to fully transparent in the last 1 second of "life"
		particleParams.deathTransparencyFadeTimeSeconds = 2.0f;

		particleParams.position = glm::vec3( 10.0f, -10.0f, 0.0f);
		particleParams.minInitialVelocity = glm::vec3( -0.5  , 0.5 , -0.5 );
		particleParams.maxInitialVelocity = glm::vec3(  0.5  , 1.0 ,  0.5 ) ;
		particleParams.acceleration = glm::vec3( 0.0f, 1.0f,  0.0f );
		particleParams.minNumberNewParticles = 2;
		particleParams.maxNumberNewParticles = 5;
		particleParams.minTimeBetweenParticleGenerationSeconds = 0.1f;
		particleParams.maxTimeBetweenParticleGenerationSeconds = 0.5f;
		particleParams.minInitialScale = 0.1f;
		particleParams.maxInitialScale = 0.15f;
		particleParams.minScaleChange = 0.0f;
		particleParams.maxScaleChange = 0.1f;	// This is per frame change
	
		// Degrees per frame
		particleParams.minOrientationChangeAngleEuler = glm::vec3(-0.25f,-0.25f,-0.25f);	
		particleParams.maxOrientationChangeAngleEuler = glm::vec3(+0.25f,+0.25f,+0.25f);

		pPE_Smoke_01->Init(particleParams);

		::g_map_pParticleEmitters["Smoke01"] = pPE_Smoke_01;

	}//ENDOF: Star shaped smoke emitter


	{//STARTOF: Flat 2D quad smoke emitter

		cParticleEmitter* pPE_Smoke_02 = new cParticleEmitter();

		// This takes a while
		cParticleEmitter::sParticleCreationParams particleParams;
		particleParams.totalNumberOfParticles = 500;	// Max # of particles ALIVE at ONE TIME
		particleParams.minLifeTime = 5.0f;
		particleParams.maxLifeTime = 10.0f;
		// Will fade to fully transparent in the last 1 second of "life"
		particleParams.deathTransparencyFadeTimeSeconds = 2.0f;

		particleParams.position = glm::vec3( 20.0f, -10.0f, 0.0f);
		particleParams.minInitialVelocity = glm::vec3( -0.5  , 0.5 , 0.01 );
		particleParams.maxInitialVelocity = glm::vec3(  0.5  , 1.0 , 0.01 ) ;
		particleParams.acceleration = glm::vec3( 0.0f, 1.0f,  0.0f );
		particleParams.minNumberNewParticles = 2;
		particleParams.maxNumberNewParticles = 5;
		particleParams.minTimeBetweenParticleGenerationSeconds = 0.1f;
		particleParams.maxTimeBetweenParticleGenerationSeconds = 0.5f;
		particleParams.minInitialScale = 0.1f;
		particleParams.maxInitialScale = 0.15f;
		particleParams.minScaleChange = 0.0f;
		particleParams.maxScaleChange = 0.1f;	// This is per frame change
	
		// Degrees per frame
		// HERE is the difference between the star shaped and the "flat" imposter
		// The imposter is 2D and is only rotating around the Z axis.
		// If you are looking at it the correct way, it looks great, but if not, 
		//  you can see it's an imposter. 
		// So you'd need to align these to face the camera. 
		// Note: we aren't doing that here, but when we draw them...
		// (this only sets up the rotation around the z axis, which we'll assume is 
		//  along the axis the camera is looking down)
		particleParams.minOrientationChangeAngleEuler = glm::vec3(0.0f,0.0f,-0.25f);	
		particleParams.maxOrientationChangeAngleEuler = glm::vec3(0.0f,0.0f,+0.25f);
		particleParams.minIntitialOrientationAngleEuler = glm::vec3(90.0f,0.0f,0.0f);
		particleParams.maxIntitialOrientationAngleEuler = glm::vec3(90.0f,0.0f,0.0f);


		pPE_Smoke_02->Init(particleParams);

		::g_map_pParticleEmitters["Smoke02"] = pPE_Smoke_02;

	}//ENDOF: Flat 2D quad smoke emitter


	{//STARTOF: Flat 2D "plasma explosion" imposter
		
		// This is just like the smoke one, but:
		// - All of the particles go off at once
		// - They move faster
		// - There's no "gravity" (or whatever)

		cParticleEmitter* pPE_Plasma_01 = new cParticleEmitter();

		// This takes a while
		cParticleEmitter::sParticleCreationParams particleParams;
		particleParams.totalNumberOfParticles = 100;	// Max # of particles ALIVE at ONE TIME
		particleParams.minLifeTime = 6.0f;
		particleParams.maxLifeTime = 15.0f;
		// Will fade to fully transparent in the last 1 second of "life"
		particleParams.deathTransparencyFadeTimeSeconds = 5.0f;

		particleParams.position = glm::vec3( 50.0f, -10.0f, 0.0f);
		// 
		const float EXPLOSIIONSPEED = 1.0f;

		// We only want this thing to "explode" in the XY plane 
		// (it's a "flat" explosion, since we're using 2D quads)
		particleParams.minInitialVelocity = glm::vec3( -EXPLOSIIONSPEED, -EXPLOSIIONSPEED, 0.0f );
		particleParams.maxInitialVelocity = glm::vec3(  EXPLOSIIONSPEED, EXPLOSIIONSPEED, 0.0f ) ;
//		particleParams.acceleration = glm::vec3( 2.0f, 2.0f,  2.0f );
		particleParams.minNumberNewParticles = 100;
		particleParams.maxNumberNewParticles = 100;
		particleParams.minTimeBetweenParticleGenerationSeconds = 5.0f;
		particleParams.maxTimeBetweenParticleGenerationSeconds = 5.0f;
		particleParams.minInitialScale = 0.01f;
		particleParams.maxInitialScale = 0.1f;
		particleParams.minScaleChange = 1.0f;
		particleParams.maxScaleChange = 5.0f;	// This is per frame change
	
		// Degrees per frame
		// HERE is the difference between the star shaped and the "flat" imposter
		// The imposter is 2D and is only rotating around the Z axis.
		// If you are looking at it the correct way, it looks great, but if not, 
		//  you can see it's an imposter. 
		// So you'd need to align these to face the camera. 
		// Note: we aren't doing that here, but when we draw them...
		// (this only sets up the rotation around the z axis, which we'll assume is 
		//  along the axis the camera is looking down)
		particleParams.minIntitialOrientationAngleEuler = glm::vec3(90.0f,0.0f,0.0f);
		particleParams.maxIntitialOrientationAngleEuler = glm::vec3(90.0f,0.0f,0.0f);
		// OK, this is a little odd, but here's why we are rotating around the "Y" 
		// instead of the "Z": 
		// The original model is on the XZ plane, so if we load it, it will be "flat"
		//  parallel to the ground. To change this, we are rotating the quad 90 degrees
		//  around the X, so that it faces "up and down" (see above).
		// But, when we do that, the local "Y" axis is ALSO rotated around the X, 
		//  making it parallel to the Z (the Z is now the Y, but upside down). 
		// Load the quad into mesh lab, turn on the axis, and rotate it towards you, 
		//  along the x axis, 90 degrees, and you'll see the Y and Z rotate with it.
		// To get around this, you can edit the quad to be "vertical" (XY plane), perhaps?
		particleParams.minOrientationChangeAngleEuler = glm::vec3(0.0f,-0.25f, 0.0f );	
		particleParams.maxOrientationChangeAngleEuler = glm::vec3(0.0f,+0.25f, 0.0f );


		pPE_Plasma_01->Init(particleParams);

		::g_map_pParticleEmitters["PlasmaExplosion"] = pPE_Plasma_01;

	}//ENDOF: Flat 2D quad smoke emitter

	return;
}








// LoJam code:
void LoadModelsIntoScene_LOJAM( std::vector<cMeshSceneObject*> &vec_pObjectsToDraw )
{

	LoadPlayerModels_LOJAM(vec_pObjectsToDraw);



	{	// The 2D 2 triangle "flat" imposter object
		cMeshSceneObject* pTestObject = new cMeshSceneObject();
		pTestObject->setDiffuseColour( glm::vec3( 1.0f, 1.0f, 1.0f ) );
		pTestObject->setAlphaTransparency( 1.0f );
		pTestObject->friendlyName = "GentleTerrain";
		pTestObject->position = glm::vec3(0.0f, -20.0f, 0.0f);
		pTestObject->uniformScale = 1.0f;
//		pTestObject->meshName = "Gentle_Terrain_xyz_uv.ply";	
		pTestObject->vecLODMeshs.push_back(sLODInfo("Gentle_Terrain_xyz_uv.ply"));		// "Utah_Teapot_xyz.ply";
		//pTestObject->bIsWireFrame = true;
		//pTestObject->bDontLight = true;

		sTextureInfo testObjectTexture;
		testObjectTexture.name = "grass.bmp";
		testObjectTexture.strength = 1.0f;

		pTestObject->vecTextures.push_back( sTextureInfo(testObjectTexture) );
		pTestObject->uniformScale = 1.0f;
		vec_pObjectsToDraw.push_back( pTestObject );
	}	

	{	// This will be our "skybox" object.
		// (could also be a cube, or whatever)
		cMeshSceneObject* pSkyBoxObject = new cMeshSceneObject();
		pSkyBoxObject->setDiffuseColour( glm::vec3( 1.0f, 105.0f/255.0f, 180.0f/255.0f ) );
		pSkyBoxObject->bUseVertexColour = false;
		pSkyBoxObject->friendlyName = "SkyBoxObject";
		pSkyBoxObject->uniformScale = 5000.0f;
//		pSkyBoxObject->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
		pSkyBoxObject->vecLODMeshs.push_back(sLODInfo("Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply"));		// "Utah_Teapot_xyz.ply";
//		pSkyBoxObject->bIsWireFrame = true;

		// Invisible until I need to draw it
		pSkyBoxObject->bIsVisible = false;

		vec_pObjectsToDraw.push_back( pSkyBoxObject );
	}

	{	// This sphere is the tiny little debug sphere
		cMeshSceneObject* pDebugSphere = new cMeshSceneObject();
		pDebugSphere->position = glm::vec3( 0.0f, 0.0f, 0.0f );
//		pDebugSphere->objColour = glm::vec3( 0.0f, 1.0f, 0.0f );
		pDebugSphere->setDiffuseColour( glm::vec3(0.0f, 1.0f, 0.0f) );
		pDebugSphere->friendlyName = "DebugSphere";
		pDebugSphere->uniformScale = 0.1f;
//		pDebugSphere->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv.ply";		// "Sphere_320_faces_xyz.ply";
		pDebugSphere->vecLODMeshs.push_back(sLODInfo("Sphere_320_faces_xyz_n_GARBAGE_uv.ply"));		// "Utah_Teapot_xyz.ply";
		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->bIsVisible = false;
		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;

		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		vec_pObjectsToDraw.push_back( pDebugSphere );
	}

	return;
}