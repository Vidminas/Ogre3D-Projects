#ifndef FRAMEWORK_HPP_INCLUDED
#define FRAMEWORK_HPP_INCLUDED

class Framework : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public OgreBites::SdkTrayListener
{
public:
    Framework();
    virtual ~Framework();
    virtual void start();

protected:
    //Ogre
    Ogre::Root*          mRoot;
    Ogre::RenderWindow*  mWindow;
    Ogre::SceneManager*  mSceneMgr;
    Ogre::OverlaySystem* mOverlaySystem;
    Ogre::Camera*        mCamera;
    Ogre::Viewport*      mViewport;
    bool                 mShutDown;

    //Camera handler
    static const Ogre::Real mRotateSpeed = 0.15f;
    static const Ogre::Real mCamSpeed = 1500;
    Ogre::Vector3           mCamDir;
    bool                    mFastCam;

    //Static Plugins
    Ogre::GLPlugin*   mGLPlugin;
    Ogre::D3D9Plugin* mD3D9Plugin;

    //OgreBites
    OgreBites::InputContext    mInputContext;
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::ParamsPanel*    mDetailsPanel;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*        mMouse;
    OIS::Keyboard*     mKeyboard;

    virtual void createScene() = 0;
    virtual void destroyScene() = 0;
    virtual bool setup();

    bool configure();
    void createSceneManager();
    void createCamera();
    void createViewports();
    void setupResources();

    virtual void createResourceListener();
    void createFrameListener();
    bool frameRenderingQueued (const Ogre::FrameEvent& evt);

    virtual void injectFrameListener() = 0;
    virtual bool injectFrameRendering (const Ogre::FrameEvent& evt) = 0;

    bool keyPressed (const OIS::KeyEvent& arg);
    bool keyReleased (const OIS::KeyEvent& arg);
    bool mouseMoved (const OIS::MouseEvent& arg);
    bool mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    virtual bool injectKeyPressed (const OIS::KeyEvent& arg) = 0;
    virtual bool injectKeyReleased (const OIS::KeyEvent& arg) = 0;
    virtual bool injectMouseMoved (const OIS::MouseEvent& arg) = 0;
    virtual bool injectMousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id) = 0;
    virtual bool injectMouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id) = 0;

    // Ogre::WindowEventListener
    virtual void windowResized (Ogre::RenderWindow* rw); // Adjust mouse clipping area
    virtual void windowClosed  (Ogre::RenderWindow* rw); // Unattach OIS before window shutdown (very important under Linux) and clean up
};

#endif // FRAMEWORK_HPP_INCLUDED
