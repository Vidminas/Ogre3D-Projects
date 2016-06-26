#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

class Entity
{
public:
    Entity (Ogre::SceneManager* sceneMgr, Ogre::Vector3 position, Ogre::Real speed);
    virtual ~Entity();
    Ogre::SceneNode* getSceneNode() const;
    const Ogre::AxisAlignedBox getWorldBoundingBox() const;
    void addDestination (Ogre::Vector3 position);
    void handleTerrainCollision (Ogre::TerrainGroup*& terrainGroup);
    void handleEntityCollision (Entity* entity2);
    virtual void die() = 0;
    const bool isDead (Ogre::Real timeSinceLastFrame);
    const bool isSelected() const;
    virtual void animation (Ogre::Real timeSinceLastFrame) = 0;

protected:
    Ogre::Entity*         mEntity;
    Ogre::SceneNode*      mNode;
    Ogre::AnimationState* mAnimationState;
    bool                  mWaitFrame;
    bool                  mWalking;
    Ogre::Real            mWalkSpeed;
    Ogre::Real            mDistance;
    Ogre::Vector3         mDestination;
    Ogre::Vector3         mDirection;
    bool                  mDead;

    bool nextLocation();

private:
    std::deque<Ogre::Vector3> mWalkList;
    Ogre::Real                mTimeSinceDeath;
    static const Ogre::Real   mDecayTimer = 3;
    static const Ogre::Real   mCollisionDistance = 20;
};

#endif // ENTITY_HPP_INCLUDED
