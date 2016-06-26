#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager*& primarySceneMgr, Ogre::SceneManager*& secondarySceneMgr)
{
    // Set up the space SceneManager
    primarySceneMgr -> setSkyBox (true, "Examples/SpaceSkyBox");

    // Set up the Cloudy SceneManager
    secondarySceneMgr -> setSkyDome (true, "Examples/CloudySky", 5, 8);
}

int main()
{
    go();

    return 0;
}
