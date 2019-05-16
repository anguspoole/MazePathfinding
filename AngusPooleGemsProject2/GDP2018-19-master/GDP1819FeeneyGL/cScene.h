#ifndef _sScene_HG_
#define _sScene_HG_

// This represents all the things in a "scene".
// Objects are identified by ID or name

#include "cGameEntity.h"
#include <vector>
#include <map>
#include <string>

class cScene
{
public:
	cScene();
	~cScene();

	std::string name;

	bool LoadSceneFromFile(std::string fileName);
	bool SaveSceneToFile(std::string fileName);

	// These are used by the various managers to load assets
	void GetListOfShadersInUse( std::vector<std::string> &vecShadersInUse );
	void GetListOfMeshesInUse( std::vector<std::string> &vecMeshesInUse );
	void GetListOfSkinnedMeshesInUse( std::vector<std::string> &vecSkinnedMeshesInUse );
	void GetListOf2DTexturesInUse( std::vector<std::string> &vec2DTexturesInUse );
	void GetListOfCubeMapTexturesInUse( std::vector<std::string> &vecCubeMapsInUse );

	bool AddGameEntityToScene( cGameEntity newGameEntity );
	bool AddGameEntityToScene( cGameEntity* pNewGameEntity );

	bool GetMeshSceneObjectsToDraw( std::vector<cMeshSceneObject*> &vec_pMeshSceneObjects );
	bool GetSkinnedMeshObjectsToDraw( std::vector<cSimpleAssimpSkinnedMesh*> &vec_pSkinnedMeshObjects );


private:
//	std::vector<cGameEntity*> vec_pGameEntities;
	std::map< unsigned int /*unique ID*/, cGameEntity*> m_map_pGameEntities;
};

#endif
