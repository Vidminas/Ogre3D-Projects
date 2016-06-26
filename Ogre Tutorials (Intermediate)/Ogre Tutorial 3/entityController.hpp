#ifndef ENTITYCONTROLLER_HPP_INCLUDED
#define ENTITYCONTROLLER_HPP_INCLUDED

class EntityController
{
private:
    std::vector<Entity*> mEntities;
    std::vector<unsigned> mActiveEntities;
    unsigned mCurEntity;
    unsigned mRobots;
    unsigned mNinjas;

    static const unsigned mRobotSpeed = 200;
    static const unsigned mNinjaSpeed = 250;

    enum QueryFlags
    {
        ROBOT_MASK = 1 << 0,
        NINJA_MASK = 1 << 1
    };

public:
    EntityController();
    ~EntityController();

    const Entity* getEntity (const unsigned index) const;
    const Ogre::String getNumRobots() const;
    const Ogre::String getNumNinjas() const;
    const bool empty() const;

    void newEntity (bool type, Ogre::SceneManager* sceneMgr, Ogre::Vector3 position);
    void switchToEntity (const Ogre::String& name);
    void deactivateAllEntities();
    void activateAllEntities();
    void kill();
    void reposition (Ogre::Vector3 position);
    void moveEntities (Ogre::Vector3 position);

    void handleEntityCollisions();
    void animate (Ogre::Real timeSinceLastFrame, bool detectCollision, Ogre::TerrainGroup*& terrainGroup);
};

#endif // ENTITYCONTROLLER_HPP_INCLUDED
