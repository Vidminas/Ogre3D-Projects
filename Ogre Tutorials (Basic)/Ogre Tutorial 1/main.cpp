#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager*& sceneMgr)
{
    sceneMgr -> setAmbientLight (Ogre::ColourValue (1.0, 1.0, 1.0) );

    Ogre::Entity* ogreHead = sceneMgr -> createEntity ("Head", "ogrehead2.mesh");
    Ogre::SceneNode* headNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("HeadNode");
    headNode -> attachObject (ogreHead);

    headNode -> yaw (Ogre::Degree (-90) );

    Ogre::Entity* ogreHead2 = sceneMgr -> createEntity ("Head2", "ogrehead2.mesh");
    Ogre::SceneNode* headNode2 = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("HeadNode2", Ogre::Vector3 (100, 0, 0) );
    headNode2 -> attachObject (ogreHead2);

    headNode2 -> pitch (Ogre::Degree (-90) );

    Ogre::Entity* ogreHead3 = sceneMgr -> createEntity ("Head3", "ogrehead2.mesh");
    Ogre::SceneNode* headNode3 = sceneMgr -> getRootSceneNode() -> createChildSceneNode ("HeadNode3", Ogre::Vector3 (200, 0, 0) );
    headNode3 -> attachObject (ogreHead3);

    headNode3 -> roll (Ogre::Degree (-90) );

    Ogre::Light* light = sceneMgr -> createLight ("MainLight");
    light -> setPosition (20, 80, 50);
}

int main()
{
    go();

    return 0;
}
