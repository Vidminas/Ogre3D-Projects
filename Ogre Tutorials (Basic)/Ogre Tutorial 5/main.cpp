#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager*& sceneMgr)
{
    sceneMgr -> setAmbientLight (Ogre::ColourValue (0.2, 0.2, 0.2) );

    Ogre::Entity* tudorEntity = sceneMgr -> createEntity ("tudorhouse.mesh");
    Ogre::SceneNode* node = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("Node");
    node -> attachObject (tudorEntity);

    Ogre::Light* light = sceneMgr -> createLight ("Light1");
    light -> setType (Ogre::Light::LT_POINT);
    light -> setPosition (Ogre::Vector3 (250, 150, 250) );
    light -> setDiffuseColour (Ogre::ColourValue::White);
    light -> setSpecularColour (Ogre::ColourValue::White);

    node = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("CamNode1", Ogre::Vector3 (1200, -370, 0) );
    node -> yaw (Ogre::Degree (90) );

    node -> attachObject (sceneMgr -> getCamera ("Camera") );

    node = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("CamNode2", Ogre::Vector3 (-500, -370, 1000) );
    node -> yaw (Ogre::Degree (-30) );
}

int main()
{
    go();

    return 0;
}
