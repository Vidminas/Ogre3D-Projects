#include <OgreWindowEventUtilities.h>
#include <OgreFrameListener.h>
#include <SdkTrays.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>
#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "framework.hpp"
#include "entity.hpp"
#include "entityController.hpp"
#include "robot.hpp"
#include "ninja.hpp"
#include "main.hpp"

OgreTutorial::OgreTutorial() :
    Framework(),
    mInfoPanel        (0),
    mRaySceneQuery    (0),
    mLight            (0),
    mMode             (0),
    mDetectCollision  (true),
    mTerrain          (0),
    mTerrainGlobals   (0),
    mTerrainGroup     (0),
    mTerrainsImported (false)
{
    mEntityController = new EntityController();
}

OgreTutorial::~OgreTutorial()
{
    mSceneMgr -> destroyQuery (mRaySceneQuery);
    if (mEntityController) delete mEntityController;
    if (mTerrainGlobals)   delete mTerrainGlobals;
    if (mTerrainGroup)     delete mTerrainGroup;
}

void OgreTutorial::injectFrameListener()
{
    Ogre::StringVector info;
    info.push_back ("Collision mode (Z to toggle)");
    info.push_back ("Insert Mode (X to toggle)");
    info.push_back ("Robots");
    info.push_back ("Ninjas");
    mInfoPanel = mTrayMgr -> createParamsPanel (OgreBites::TL_TOPLEFT, "InfoPanel", 300, info);
    mInfoPanel -> setParamValue (0, "On");
    mInfoPanel -> setParamValue (1, "Robots");
    mInfoPanel -> setParamValue (2, "0");
    mInfoPanel -> setParamValue (3, "0");

    mRaySceneQuery = mSceneMgr -> createRayQuery (Ogre::Ray() );
}

bool OgreTutorial::injectFrameRendering (const Ogre::FrameEvent& evt)
{
    // Handle camera collision
    if (mCamDir != Ogre::Vector3::ZERO)
    {
        Ogre::Vector3 camPos = mCamera -> getPosition();
        Ogre::Ray camRay (Ogre::Vector3 (camPos.x, 5000.0, camPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);

        Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (camRay);

        if (result.terrain)
        {
            Ogre::Real terrainHeight = result.position.y;

            if (camPos.y < (terrainHeight + mCollisionDistance) )
                mCamera -> setPosition (camPos.x, terrainHeight + mCollisionDistance, camPos.z);
        }
    }

    mEntityController -> animate (evt.timeSinceLastFrame, mDetectCollision, mTerrainGroup);

    return true;
}

void OgreTutorial::createScene()
{
    mSceneMgr -> setAmbientLight (Ogre::ColourValue (0.5, 0.5, 0.5) );
    mSceneMgr -> setSkyDome (true, "Examples/CloudySky", 5, 8);

    mLight = mSceneMgr -> createLight ("SceneLight");
    mLight -> setType (Ogre::Light::LT_DIRECTIONAL);

    Ogre::Vector3 lightDir (0.55, 0.3, 0.75);
    lightDir.normalise();
    mLight -> setDirection (lightDir);

    mLight -> setDiffuseColour (Ogre::ColourValue (0.4, 0.4, 0.4) );
    mLight -> setSpecularColour (Ogre::ColourValue (0.2, 0.2, 0.2) );

    setupTerrain();
}

void OgreTutorial::destroyScene()
{
    //Nothing needed
}

int main()
{
    OgreTutorial test;
    test.start();

    return 0;
}
