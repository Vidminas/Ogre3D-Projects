#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager*& sceneMgr)
{
    sceneMgr -> setAmbientLight (Ogre::ColourValue (1.0, 1.0, 1.0) );
    sceneMgr -> setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

    Ogre::Entity* entNinja = sceneMgr -> createEntity ("Ninja", "ninja2.mesh");
    entNinja -> setCastShadows (true);
    sceneMgr -> getRootSceneNode() -> createChildSceneNode (Ogre::Vector3 (0, 0, 0) ) -> attachObject (entNinja);

    Ogre::Plane plane (Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane ("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* entGround = sceneMgr -> createEntity ("GroundEntity", "ground");
    sceneMgr -> getRootSceneNode() -> createChildSceneNode() -> attachObject (entGround);

    entGround -> setMaterialName ("Examples/Rockwall");
    entGround -> setCastShadows (false);

    Ogre::Light* pointLight = sceneMgr -> createLight ("pointLight");
    pointLight -> setType (Ogre::Light::LT_POINT);
    pointLight -> setPosition (Ogre::Vector3 (0, 150, 250) );
}

int main()
{
    go();

    return 0;
}
