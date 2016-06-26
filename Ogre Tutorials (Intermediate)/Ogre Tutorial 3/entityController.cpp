#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <Terrain/OgreTerrainGroup.h>

#include "entity.hpp"
#include "entityController.hpp"
#include "robot.hpp"
#include "ninja.hpp"

EntityController::EntityController() :
    mEntities       (0),
    mActiveEntities (0),
    mCurEntity      (0),
    mRobots         (0),
    mNinjas         (0)
{

}

EntityController::~EntityController()
{
    for (unsigned i (0); i < mEntities.size(); i++)
        delete mEntities.at (i);

    mEntities.clear();
}

const Entity* EntityController::getEntity (unsigned index) const
{
    return mEntities.at (index);
}

const Ogre::String EntityController::getNumRobots() const
{
    std::ostringstream numRobots;
    numRobots << mRobots;
    return numRobots.str();
}

const Ogre::String EntityController::getNumNinjas() const
{
    std::ostringstream numNinjas;
    numNinjas << mNinjas;
    return numNinjas.str();
}

const bool EntityController::empty() const
{
    return mEntities.empty();
}

void EntityController::newEntity (bool type, Ogre::SceneManager* sceneMgr, Ogre::Vector3 position)
{
    if (!type)
    {
        mEntities.push_back (new Robot (sceneMgr, position, mRobotSpeed) );
        mEntities.back() -> getSceneNode() -> getAttachedObject (0) -> setQueryFlags (ROBOT_MASK);
        mRobots++;
    }

    else
    {
        mEntities.push_back (new Ninja (sceneMgr, position, mNinjaSpeed, mNinjas) );
        mEntities.back() -> getSceneNode() -> getAttachedObject (0) -> setQueryFlags (NINJA_MASK);
        mNinjas++;
    }

    mEntities.back() -> getSceneNode() -> showBoundingBox (true);
    mCurEntity = mEntities.size() - 1;
    mActiveEntities.push_back (mCurEntity);
}

void EntityController::switchToEntity (const Ogre::String& name)
{
    bool found (false);

    for (unsigned index (0); index < mEntities.size() && !found; index++)
    {
        if (mEntities.at (index) -> getSceneNode() -> getAttachedObject (0) -> getName() == name)
        {
            mEntities.at (index) -> getSceneNode() -> showBoundingBox (true);
            mCurEntity = index;
            mActiveEntities.push_back (index);
            found = true;
        }
    }
}

void EntityController::activateAllEntities()
{
    for (unsigned index (0); index < mEntities.size(); index++)
    {
        mEntities.at (index) -> getSceneNode() -> showBoundingBox (true);
        mActiveEntities.push_back (index);
    }

}

void EntityController::deactivateAllEntities()
{
    for (unsigned index (0); index < mActiveEntities.size(); index++)
        mEntities.at (mActiveEntities.at (index) ) -> getSceneNode() -> showBoundingBox (false);

    mActiveEntities.clear();
}

void EntityController::kill()
{
    deactivateAllEntities();

    for (unsigned i (0); i < mEntities.size(); i++)
        mEntities.at (i) -> die();

    mRobots = 0;
    mNinjas = 0;
}

void EntityController::reposition (Ogre::Vector3 position)
{
    mEntities.at (mCurEntity) -> getSceneNode() -> setPosition (position);
}

void EntityController::moveEntities (Ogre::Vector3 position)
{
    for (unsigned i (0); i < mActiveEntities.size(); i++)
        mEntities.at (mActiveEntities.at (i) ) -> addDestination (position);
}

void EntityController::handleEntityCollisions()
{

}

void EntityController::animate (Ogre::Real timeSinceLastFrame, bool detectCollision, Ogre::TerrainGroup*& terrainGroup)
{
    for (unsigned i (0); i < mEntities.size(); i++)
    {
        if (mEntities.at (i) -> isDead (timeSinceLastFrame) )
        {
            delete mEntities.at (i);
            mEntities.erase (mEntities.begin() + i);
        }

        else
        {
            mEntities.at (i) -> animation (timeSinceLastFrame);

            if (detectCollision)
            {
                for (unsigned short j (0); j < mEntities.size(); j++)
                {
                    if (j == i)
                        continue;

                    mEntities.at (i) -> handleEntityCollision (mEntities.at (j) );
                }

                mEntities.at (i) -> handleTerrainCollision (terrainGroup);
            }
        }
    }
}
