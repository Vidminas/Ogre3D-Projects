#ifndef ROBOT_HPP_INCLUDED
#define ROBOT_HPP_INCLUDED

class Robot : public Entity
{
public:
    Robot (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed);
    ~Robot();

    void die();
    void animation (Ogre::Real timeSinceLastFrame);
};

#endif // ROBOT_HPP_INCLUDED
