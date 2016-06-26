#ifndef LISTENERS_HPP_INCLUDED
#define LISTENERS_HPP_INCLUDED

class Listeners : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
    Listeners (Ogre::Root*& root, Ogre::RenderWindow*& window, Ogre::Camera*& camera,
                   Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported);
    ~Listeners();
    void createFrameListener();

private:
    Ogre::Root*&                mRoot;
    Ogre::RenderWindow*&        mWindow;
    Ogre::Camera*&              mCamera;
    OgreBites::InputContext     mInputContext;
    OgreBites::SdkTrayManager*  mTrayMgr;
    OgreBites::ParamsPanel*     mDetailsPanel;
    bool                        mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*        mMouse;
    OIS::Keyboard*     mKeyboard;

    // Camera
    Ogre::Real              mMove;
    static const Ogre::Real mRotate = 0.15f;
    static const Ogre::Real mCollisionDistance = 25;
    Ogre::Vector3           mDirection; // Value to move in the correct direction
    bool                    mDetectCollision;
    void handleCameraCollision();

    // Entities
    enum QueryFlags
    {
        ROBOT_MASK = 1 << 0,
        NINJA_MASK = 1 << 1
    };
    bool                 mRobotMode;
    std::vector<Entity*> mEntities;
    int                  mCurEntity;
    unsigned int         mRobots;
    unsigned int         mNinjas;
    bool                 mMovableFound;
    Ogre::RaySceneQuery* mRaySceneQuery;

    // Terrain stuff
    OgreBites::ParamsPanel*      mInfoPanel;
    OgreBites::CheckBox*         mFastCamera;
    Ogre::TerrainGlobalOptions*& mTerrainGlobals;
    Ogre::TerrainGroup*&         mTerrainGroup;
    bool&                        mTerrainsImported;

    // Ogre::FrameListener
    bool frameRenderingQueued (const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    bool keyPressed  (const OIS::KeyEvent& arg);
    bool keyReleased (const OIS::KeyEvent& arg);

    // OIS::MouseListener
    bool mouseMoved    (const OIS::MouseEvent& arg);
    bool mousePressed  (const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    // Ogre::WindowEventListener
    void windowResized (Ogre::RenderWindow* rw); // Adjust mouse clipping area
    void windowClosed  (Ogre::RenderWindow* rw); // Unattach OIS before window shutdown (very important under Linux) and clean up
};

#endif // LISTENERS_HPP_INCLUDED
