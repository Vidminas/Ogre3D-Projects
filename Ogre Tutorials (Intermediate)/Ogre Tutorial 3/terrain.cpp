#include <OgreWindowEventUtilities.h>
#include <OgreFrameListener.h>
#include <SdkTrays.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "framework.hpp"
#include "entity.hpp"
#include "entityController.hpp"
#include "main.hpp"

void OgreTutorial::initBlendMaps (Ogre::Terrain* terrain)
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

void OgreTutorial::configureTerrainDefaults()
{
    // Configure global
    mTerrainGlobals -> setMaxPixelError (8);
    // testing composite map
    mTerrainGlobals -> setCompositeMapDistance (3000);

    // Important to set these so that the mTerrain knows what to use for derived (non-realtime) data
    mTerrainGlobals -> setLightMapDirection (mLight -> getDerivedDirection() );
    mTerrainGlobals -> setCompositeMapAmbient (mSceneMgr -> getAmbientLight() );
    mTerrainGlobals -> setCompositeMapDiffuse (mLight -> getDiffuseColour() );

    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup -> getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // Textures
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

void OgreTutorial::getTerrainImage (bool flipX, bool flipY, Ogre::Image& img)
{
    img.load ("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

void OgreTutorial::defineTerrain (long x, long y)
{
    Ogre::String filename = mTerrainGroup -> generateFilename (x, y);

    if (Ogre::ResourceGroupManager::getSingleton().resourceExists (mTerrainGroup -> getResourceGroup(), filename) )
    {
        mTerrainGroup -> defineTerrain (x, y);
    }

    else
    {
        Ogre::Image img;
        getTerrainImage (x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup -> defineTerrain (x, y, &img);
        mTerrainsImported = true;
    }
}

void OgreTutorial::setupTerrain()
{
    mTerrainGlobals = new Ogre::TerrainGlobalOptions();

    mTerrainGroup = new Ogre::TerrainGroup (mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup -> setFilenameConvention (Ogre::String ("BasicTutorial3Terrain"), Ogre::String ("dat") );
    mTerrainGroup -> setOrigin (Ogre::Vector3::ZERO);

    configureTerrainDefaults();

    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain (x, y);

    // sync load since we want everything in place when we start
    mTerrainGroup -> loadAllTerrains (true);

    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup -> getTerrainIterator();
        while (ti.hasMoreElements() )
        {
            Ogre::Terrain* t = ti.getNext() -> instance;
            initBlendMaps (t);
        }

        mTerrainGroup -> saveAllTerrains (true);
        mTerrainsImported = false;
    }

    mTerrainGroup -> freeTemporaryResources();
}
