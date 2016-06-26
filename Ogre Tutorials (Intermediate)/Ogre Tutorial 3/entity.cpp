#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreAnimationState.h>
#include <Terrain/OgreTerrainGroup.h>

#include "entity.hpp"

Entity::Entity (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed) :
    mEntity         (0),
    mNode           (0),
    mAnimationState (0),
    mWaitFrame      (false),
    mWalking        (false),
    mWalkSpeed      (speed),
    mDistance       (0),
    mDestination    (Ogre::Vector3::ZERO),
    mDirection      (Ogre::Vector3::ZERO),
    mDead           (false),
    mTimeSinceDeath (0)
{
    mNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode (position);
}

Entity::~Entity()
{
    // Do nothing, needed to shut up the compiler
}

Ogre::SceneNode* Entity::getSceneNode() const
{
    return mNode;
}

const Ogre::AxisAlignedBox Entity::getWorldBoundingBox() const
{
    //return mEntity -> getWorldBoundingBox();
    return mNode -> _getWorldAABB();
}

void Entity::addDestination (Ogre::Vector3 position)
{
    mWalkList.push_back (position);
}

void Entity::handleTerrainCollision (Ogre::TerrainGroup*& terrainGroup)
{
    Ogre::Vector3 entPos = mNode -> getPosition();
    Ogre::Ray entRay (Ogre::Vector3 (entPos.x, entPos.y, entPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);

    Ogre::TerrainGroup::RayResult result = terrainGroup -> rayIntersects (entRay);

    if (result.terrain)
    {
        Ogre::Real terrainHeight = result.position.y;

        if (entPos.y != (terrainHeight + mCollisionDistance) )
            mNode -> setPosition (entPos.x, terrainHeight + mCollisionDistance, entPos.z);
    }
}

void Entity::handleEntityCollision (Entity* entity2)
{
    Ogre::AxisAlignedBox aab = getWorldBoundingBox().intersection (entity2 -> getWorldBoundingBox() );

    if (!aab.isNull() )
    {
        Ogre::Vector3 diff = entity2 -> getSceneNode() -> getPosition() - mNode -> getPosition();
        Ogre::Vector3 dir = diff.normalisedCopy();
        Ogre::Vector3 pene = aab.getMaximum() - aab.getMinimum();
        Ogre::Vector3 trans = dir * Ogre::Math::Abs (pene.normalisedCopy().dotProduct (dir) ) * pene.length() * 0.5;

        mNode -> translate (-trans);
        entity2 -> getSceneNode() -> translate (trans);

        mWaitFrame = true;
    }
}

const bool Entity::isDead (Ogre::Real timeSinceLastFrame)
{
    if (mDead)
    {
        mTimeSinceDeath += timeSinceLastFrame;

        if (mTimeSinceDeath >= mDecayTimer)
            return true;
    }

    return false;
}

const bool Entity::isSelected() const
{
    return mNode -> getShowBoundingBox();
}

bool Entity::nextLocation()
{
    if (mWalkList.empty() )
        return false;

    mDestination = mWalkList.front();
    mWalkList.pop_front();
    mDirection = mDestination - mNode -> getPosition();
    Ogre::Vector3 src = mNode -> getOrientation() * Ogre::Vector3::UNIT_X;

    if ( (1.0 + src.dotProduct (mDirection) ) < 0.0001)
        mNode -> yaw (Ogre::Degree (180) );

    else
    {
        Ogre::Quaternion quat = src.getRotationTo (mDirection);
        mNode -> rotate (quat);
    }

    mDistance = mDirection.normalise();

    return true;
}
