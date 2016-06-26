#include <OgreWindowEventUtilities.h>
#include <OgreFrameListener.h>
#include <OgreSceneNode.h>
#include <SdkTrays.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "framework.hpp"
#include "entity.hpp"
#include "entityController.hpp"
#include "robot.hpp"
#include "ninja.hpp"
#include "main.hpp"

bool OgreTutorial::injectKeyPressed (const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
        case OIS::KC_Z:
            mDetectCollision = !mDetectCollision;

            if (mDetectCollision)
                mInfoPanel -> setParamValue (0, "On");

            else
                mInfoPanel -> setParamValue (0, "Off");
            break;

        case OIS::KC_X:
            mMode = !mMode;

            if (!mMode)
                mInfoPanel -> setParamValue (1, "Robots");

            else
                mInfoPanel -> setParamValue (1, "Ninjas");
            break;

         case OIS::KC_SPACE:
            mEntityController -> kill();
            mInfoPanel -> setParamValue (2, "0");
            mInfoPanel -> setParamValue (3, "0");
            break;

        default:
            break;
    }

    return true;
}

bool OgreTutorial::injectKeyReleased (const OIS::KeyEvent& arg)
{
    return true;
}

bool OgreTutorial::injectMouseMoved (const OIS::MouseEvent& arg)
{
    if (arg.state.buttonDown (OIS::MB_Left) && !mEntityController -> empty() )
    {
        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);
        Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

        if (result.terrain)
            mEntityController -> reposition (result.position);
    }

    return true;
}

bool OgreTutorial::injectMousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Left)
    {
        if (!mKeyboard -> isModifierDown (OIS::Keyboard::Ctrl) )
            mEntityController -> deactivateAllEntities();

        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);

        mRaySceneQuery -> setRay (mouseRay);
        mRaySceneQuery -> setSortByDistance (true);
        Ogre::RaySceneQueryResult& result = mRaySceneQuery -> execute();
        bool movableFound = false;

        for (Ogre::RaySceneQueryResult::iterator it = result.begin(); it != result.end() && !movableFound; it++)
        {
            movableFound = it -> movable && it -> movable -> getName() != "" && it -> movable -> getName() != "PlayerCam";

            if (movableFound)
                mEntityController -> switchToEntity (it -> movable -> getName() );
        }

        if (!movableFound)
        {
            Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

            if (result.terrain)
            {
                mEntityController -> newEntity (mMode, mSceneMgr, result.position);

                if (!mMode)
                    mInfoPanel -> setParamValue (2, mEntityController -> getNumRobots() );

                else
                    mInfoPanel -> setParamValue (3, mEntityController -> getNumNinjas() );
            }
        }
    }

    else if (id == OIS::MB_Right)
    {
        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);
        Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

        if (result.terrain)
            mEntityController -> moveEntities (result.position);
    }

    return true;
}

bool OgreTutorial::injectMouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    return true;
}
