#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <Terrain/OgreTerrainGroup.h>

#include "entity.hpp"
#include "ninja.hpp"

Ninja::Ninja (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed, const int curNinja) :
    Entity (sceneMgr, position, speed),
    mNinja (curNinja)
{
    mEntity = sceneMgr -> createEntity ("ninja.mesh");
    mNode -> attachObject (mEntity);
    mNode -> setScale (0.75, 0.75, 0.75);

    if (mNinja % 3 == 0)
        mAnimationState = mEntity -> getAnimationState ("Idle1");

    else if (mNinja % 3 == 1)
        mAnimationState = mEntity -> getAnimationState ("Idle2");

    else
        mAnimationState = mEntity -> getAnimationState ("Idle3");

    mAnimationState -> setLoop (true);
    mAnimationState -> setEnabled (true);
}

Ninja::~Ninja()
{
    mNode -> detachAllObjects();
}

void Ninja::die()
{
    if (!mDead)
    {
        if (mNinja % 2 == 0)
            mAnimationState = mEntity -> getAnimationState ("Death1");

        else
            mAnimationState = mEntity -> getAnimationState ("Death2");

        mAnimationState -> setLoop (false);
        mAnimationState -> setEnabled (true);
        mDead = true;
    }
}

void Ninja::animation (Ogre::Real timeSinceLastFrame)
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
                    if (mNinja % 3 == 0)
                        mAnimationState = mEntity -> getAnimationState ("Idle1");

                    else if (mNinja % 3 == 1)
                        mAnimationState = mEntity -> getAnimationState ("Idle2");

                    else
                        mAnimationState = mEntity -> getAnimationState ("Idle3");

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
