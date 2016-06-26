#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <Terrain/OgreTerrainGroup.h>

#include "entity.hpp"
#include "robot.hpp"

Robot::Robot (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed) :
    Entity (sceneMgr, position, speed)
{
    mEntity = sceneMgr -> createEntity ("robot.mesh");
    mNode -> attachObject (mEntity);
    mNode -> setScale (1.5, 1.5, 1.5);

    mAnimationState = mEntity -> getAnimationState ("Idle");
    mAnimationState -> setLoop (true);
    mAnimationState -> setEnabled (true);
}

Robot::~Robot()
{
    mNode -> detachAllObjects();
}

void Robot::die()
{
    if (!mDead)
    {
        mAnimationState = mEntity -> getAnimationState ("Die");
        mAnimationState -> setLoop (false);
        mAnimationState -> setEnabled (true);
        mDead = true;
    }
}

void Robot::animation (Ogre::Real timeSinceLastFrame)
{
    if (mWaitFrame)
        mWaitFrame = false;

    else
    {
        mAnimationState -> addTime (timeSinceLastFrame);

        if (!mWalking)
        {
            if (nextLocation() )
            {
                mAnimationState = mEntity -> getAnimationState ("Walk");
                mAnimationState -> setLoop (true);
                mAnimationState -> setEnabled (true);
                mWalking = true;
            }
        }

        else
        {
            Ogre::Real move = mWalkSpeed * timeSinceLastFrame;
            mDistance -= move;

            if (mDistance <= 0)
            {
                mNode -> setPosition (mDestination);

                if (!nextLocation() )
                {
                    mAnimationState = mEntity -> getAnimationState ("Idle");
                    mAnimationState -> setLoop (true);
                    mAnimationState -> setEnabled (true);
                    mWalking = false;
                }
            }

            else
                mNode -> translate (move * mDirection);
        }
    }
}

