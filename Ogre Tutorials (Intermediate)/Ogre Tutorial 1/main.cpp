#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OgreEntity.h>

#include "setup.hpp"
#include "robot.hpp"

void createScene (Ogre::SceneManager*& sceneMgr, std::vector<Robot*>& robots)
{
    sceneMgr -> setAmbientLight (Ogre::ColourValue (1.0, 1.0, 1.0) );

    for (unsigned short i (0); i < 3; i++)
        robots.push_back (new Robot (sceneMgr, Ogre::Vector3 (i * 1500, 0, 0), 100 + i * 50) );

    Ogre::SceneNode* tempNode (0);

    // Letter - L
    for (unsigned short i (0); i < 4; i++)
    {
        tempNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode (Ogre::Vector3 ( (i == 3) * 500, -100, i * 500 - (i == 3) * 500) );
        tempNode -> attachObject (sceneMgr -> createEntity ("knot2.mesh") );
        robots.at (0) -> addDestination (Ogre::Vector3 ( (i == 3) * 500, 0, i * 500 - (i == 3) * 500) );
    }

    // Letter - O
    for (unsigned short i (0); i < 3; i++)
    {
        for (unsigned short j (0); j < 2; j++)
        {
            tempNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode (Ogre::Vector3 (1500 + 500 * j, -100, i * 500) );
            tempNode -> attachObject (sceneMgr -> createEntity ("knot2.mesh") );
            robots.at (1) -> addDestination (Ogre::Vector3 (1500 + 500 * j, 0, i * 500) );
        }
    }

    // Letter - L
    for (unsigned short i (0); i < 4; i++)
    {
        tempNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode (Ogre::Vector3 (3000 + (i == 3) * 500, -100, i * 500 - (i == 3) * 500) );
        tempNode -> attachObject (sceneMgr -> createEntity ("knot2.mesh") );
        robots.at (2) -> addDestination (Ogre::Vector3 (3000 + (i == 3) * 500, 0, i * 500 - (i == 3) * 500) );
    }
}

int main()
{
    go();

    return 0;
}
