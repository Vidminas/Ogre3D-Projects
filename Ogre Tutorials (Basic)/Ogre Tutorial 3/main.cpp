#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "setup.hpp"

void configureTerrainDefaults (Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, Ogre::SceneManager*& sceneMgr, Ogre::Light* light)
{
    // Configure global
    terrainGlobals -> setMaxPixelError (8);
    // testing composite map
    terrainGlobals -> setCompositeMapDistance (3000);

    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    terrainGlobals -> setLightMapDirection (light -> getDerivedDirection() );
    terrainGlobals -> setCompositeMapAmbient (sceneMgr -> getAmbientLight() );
    terrainGlobals -> setCompositeMapDiffuse (light -> getDiffuseColour() );

    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = terrainGroup -> getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize (3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back ("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back ("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back ("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back ("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back ("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back ("growth_weirdfungus-03_normalheight.dds");
}

void getTerrainImage (bool flipX, bool flipY, Ogre::Image& img)
{
    img.load ("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

void defineTerrain (Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported, long x, long y)
{
    Ogre::String filename = terrainGroup -> generateFilename (x, y);

    if (Ogre::ResourceGroupManager::getSingleton().resourceExists (terrainGroup -> getResourceGroup(), filename) )
    {
        terrainGroup -> defineTerrain (x, y);
    }

    else
    {
        Ogre::Image img;
        getTerrainImage (x % 2 != 0, y % 2 != 0, img);
        terrainGroup -> defineTerrain (x, y, &img);
        terrainsImported = true;
    }
}

void initBlendMaps (Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain -> getLayerBlendMap (1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain -> getLayerBlendMap (2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend0 = blendMap0 -> getBlendPointer();
    float* pBlend1 = blendMap1 -> getBlendPointer();

    for (Ogre::uint16 y = 0; y < terrain -> getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;

            blendMap0 -> convertImageToTerrainSpace (x, y, &tx, &ty);
            Ogre::Real height = terrain -> getHeightAtTerrainPosition (tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp (val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;

            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp (val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }

    blendMap0 -> dirty();
    blendMap1 -> dirty();
    blendMap0 -> update();
    blendMap1 -> update();
}

void createScene (Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported, Ogre::SceneManager*& sceneMgr)
{
    //Sample SkyBox
    //sceneMgr -> setSkyBox (true, "Examples/SpaceSkyBox", 5000, false);

    //Sample SkyDome
    //sceneMgr -> setSkyDome (true, "Examples/CloudySky", 5, 8);

    //Sample SkyPlane
    Ogre::Plane plane;
    plane.d = 1000;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
    //sceneMgr -> setSkyPlane (true, plane, "Examples/SpaceSkyPlane", 1500, 50, true, 1.5f, 150, 150);
    sceneMgr -> setSkyPlane (true, plane, "Examples/CloudySky", 1500, 40, true, 1.5f, 150, 150);

    //Fog - Remember the fade colour is set in CREATE VIEWPORTS function in setup.cpp!!!
    Ogre::ColourValue fadeColour (0.9, 0.9, 0.9);
    //sceneMgr -> setFog (Ogre::FOG_LINEAR, fadeColour, 0.0, 1000, 5000);
    //sceneMgr -> setFog (Ogre::FOG_EXP, fadeColour, 0.0005);
    sceneMgr -> setFog (Ogre::FOG_EXP2, fadeColour, 0.0003);

    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (7);

    Ogre::Vector3 lightdir (0.55, -0.3, 0.75);
    lightdir.normalise();

    Ogre::Light* light = sceneMgr -> createLight ("tstLight");
    light -> setType (Ogre::Light::LT_DIRECTIONAL);
    light -> setDirection (lightdir);
    light -> setDiffuseColour (Ogre::ColourValue::White);
    light -> setSpecularColour (Ogre::ColourValue (0.4, 0.4, 0.4) );

    sceneMgr -> setAmbientLight (Ogre::ColourValue (0.2, 0.2, 0.2) );

    terrainGlobals = new Ogre::TerrainGlobalOptions();

    terrainGroup = new Ogre::TerrainGroup (sceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    terrainGroup -> setFilenameConvention (Ogre::String ("BasicTutorial3Terrain"), Ogre::String ("dat") );
    terrainGroup -> setOrigin (Ogre::Vector3::ZERO);

    configureTerrainDefaults (terrainGlobals, terrainGroup, sceneMgr, light);

    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain (terrainGroup, terrainsImported, x, y);

    // sync load since we want everything in place when we start
    terrainGroup -> loadAllTerrains (true);

    if (terrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = terrainGroup -> getTerrainIterator();
        while (ti.hasMoreElements() )
        {
            Ogre::Terrain* t = ti.getNext() -> instance;
            initBlendMaps (t);
        }
    }

    terrainGroup -> freeTemporaryResources();
}

int main()
{
    go();

    return 0;
}
