#ifndef ROBOT_HPP_INCLUDED
#define ROBOT_HPP_INCLUDED

class Robot
{
public:
    Robot (Ogre::SceneManager*& sceneMgr, Ogre::Vector3 position, Ogre::Real speed);
    ~Robot();
    void addDestination (Ogre::Vector3 destination);
    void animation (Ogre::Real timeSinceLastFrame);

//private:
    std::deque<Ogre::Vector3> mWalkList;
    Ogre::Real                mDistance;
    Ogre::Real                mWalkSpeed;
    Ogre::Vector3             mDirection;
    Ogre::Vector3             mDestination;
    Ogre::AnimationState*     mAnimationState;
    Ogre::Entity*             mEntity;
    Ogre::SceneNode*          mNode;
    bool                      mWalking;

    bool nextLocation();
};

#endif // ROBOT_HPP_INCLUDED
