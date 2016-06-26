#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "setup.hpp"

void createScene (Ogre::SceneManager* mSceneMgr)
{
    mSceneMgr -> setAmbientLight (Ogre::ColourValue (1.0, 1.0, 1.0) );
    mSceneMgr -> setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

    Ogre::Entity* mech = mSceneMgr -> createEntity ("Mech", "mech.mesh");
    mech -> setCastShadows (true);
    Ogre::SceneNode* mechNode = mSceneMgr -> getRootSceneNode() -> createChildSceneNode ("mechNode");
    mechNode -> translate (Ogre::Vector3 (0, 90, 0) );
    mechNode -> pitch (Ogre::Degree (-45) );
    mechNode -> scale (Ogre::Vector3 (10, 10, 10) );
    mechNode -> attachObject (mech);

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

void destroyScene()
{
    // Nothing yet
}

int main()
{
    go();

    return 0;
}
