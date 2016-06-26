#ifndef NINJA_HPP_INCLUDED
#define NINJA_HPP_INCLUDED

class Ninja : public Entity
{
public:
    Ninja (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed, const int curNinja);
    ~Ninja();

    void die();
    void animation (Ogre::Real timeSinceLastFrame);

private:
    const int mNinja;
};

#endif // NINJA_HPP_INCLUDED
