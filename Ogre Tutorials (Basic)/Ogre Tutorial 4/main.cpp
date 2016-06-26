#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager*& sceneMgr)
{
    sceneMgr -> setAmbientLight (Ogre::ColourValue (.25, .25, .25) );

    Ogre::Light* pointLight = sceneMgr -> createLight ("PointLight");
    pointLight -> setType (Ogre::Light::LT_POINT);
    pointLight -> setPosition (250, 150, 250);
    pointLight -> setDiffuseColour (Ogre::ColourValue::White);
    pointLight -> setSpecularColour (Ogre::ColourValue::White);

    Ogre::Entity* ninjaEntity = sceneMgr -> createEntity ("ninja2.mesh");
    Ogre::SceneNode* ninjaNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("NinjaNode");
    ninjaNode -> attachObject (ninjaEntity);
}

int main()
{
    go();

    return 0;
}
