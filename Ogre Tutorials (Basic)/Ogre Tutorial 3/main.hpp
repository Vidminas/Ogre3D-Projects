#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED

void configureTerrainDefaults (Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, Ogre::SceneManager*& sceneMgr, Ogre::Light* light);
void getTerrainImage (bool flipX, bool flipY, Ogre::Image& img);
void defineTerrain (Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported, long x, long y);
void initBlendMaps (Ogre::Terrain* terrain);
void createScene (Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported, Ogre::SceneManager*& sceneMgr);

#endif // MAIN_HPP_INCLUDED
