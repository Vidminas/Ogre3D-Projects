#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreAnimationState.h>
#include <OgreSceneManager.h>
#include "robot.hpp"

Robot::Robot (Ogre::SceneManager*& sceneMgr, Ogre::Vector3 position, Ogre::Real speed) :
    mDistance       (0),
    mWalkSpeed      (speed),
    mDirection      (Ogre::Vector3::ZERO),
    mDestination    (Ogre::Vector3::ZERO),
    mAnimationState (0),
    mEntity         (0),
    mNode           (0),
    mWalking        (false)
{
    mEntity = sceneMgr -> createEntity ("robot2.mesh");
    mNode = sceneMgr -> getRootSceneNode() -> createChildSceneNode (position);
    mNode -> attachObject (mEntity);
    mNode -> setScale (10, 10, 10);
    mNode -> yaw (Ogre::Degree (-90) );

    mAnimationState = mEntity -> getAnimationState ("Idle");
    mAnimationState -> setLoop (true);
    mAnimationState -> setEnabled (true);
}

Robot::~Robot()
{

}

void Robot::addDestination (Ogre::Vector3 destination)
{
    mWalkList.push_back (destination);
}

bool Robot::nextLocation()
{
    if (mWalkList.empty() )
        return false;

    mDestination = mWalkList.front();
    mWalkList.pop_front();
    mDirection = mDestination - mNode -> getPosition();

    mDistance = mDirection.normalise();
    mWalking = true;

    Ogre::Vector3 src = mNode -> getOrientation() * Ogre::Vector3::UNIT_X;

    if ( (1.0 + src.dotProduct (mDirection) ) < 0.0001)
    {
        mNode -> yaw (Ogre::Degree (180) );
    }

    else
    {
        Ogre::Quaternion quat = src.getRotationTo (mDirection);
        mNode -> rotate (quat);
    }

    return true;
}

void Robot::animation (Ogre::Real timeSinceLastFrame)
{
    mAnimationState -> addTime (timeSinceLastFrame);

    if (!mWalking)
    {
        if (nextLocation() )
        {
            mAnimationState = mEntity -> getAnimationState ("Walk");
            mAnimationState -> setLoop (true);
            mAnimationState -> setEnabled (true);
        }
    }

    else
    {
        Ogre::Real move = mWalkSpeed * timeSinceLastFrame;
        mDistance -= move;

        if (mDistance <= 0)
        {
            mNode -> setPosition (mDestination);
            mDirection = Ogre::Vector3::ZERO;

            if (!nextLocation() )
            {
                mAnimationState = mEntity -> getAnimationState ("Die");
                mAnimationState -> setLoop (false);
                mAnimationState -> setEnabled (true);
            }
        }

        else
        {
            mNode -> translate (move * mDirection);
        }
    }
}
