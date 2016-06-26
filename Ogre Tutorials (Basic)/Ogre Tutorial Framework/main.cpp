#include <OgreMeshManager.h>
#include <OgreEntity.h>

#include "framework.hpp"
#include "main.hpp"

OgreTutorial::OgreTutorial() : Framework()
{
    //Nothing needed
}

void OgreTutorial::createScene()
{
    mSceneMgr -> setAmbientLight (Ogre::ColourValue (1.0, 1.0, 1.0) );
    mSceneMgr -> setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

    Ogre::Entity* entNinja = mSceneMgr -> createEntity ("Ninja", "ninja2.mesh");
    entNinja -> setCastShadows (true);
    mSceneMgr -> getRootSceneNode() -> createChildSceneNode (Ogre::Vector3 (0, 0, 0) ) -> attachObject (entNinja);

    Ogre::Plane plane (Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane ("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* entGround = mSceneMgr -> createEntity ("GroundEntity", "ground");
    mSceneMgr -> getRootSceneNode() -> createChildSceneNode() -> attachObject (entGround);

    entGround -> setMaterialName ("Examples/Rockwall");
    entGround -> setCastShadows (false);

    Ogre::Light* pointLight = mSceneMgr -> createLight ("pointLight");
    pointLight -> setType (Ogre::Light::LT_POINT);
    pointLight -> setPosition (Ogre::Vector3 (0, 150, 250) );
}

void OgreTutorial::destroyScene()
{
    //Nothing needed
}

int main()
{
    OgreTutorial test;
    test.go();

    return 0;
}
