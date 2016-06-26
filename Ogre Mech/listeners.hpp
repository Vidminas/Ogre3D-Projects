#ifndef LISTENERS_HPP_INCLUDED
#define LISTENERS_HPP_INCLUDED

class Listeners : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
    Listeners (Ogre::Root* mRoot, Ogre::RenderWindow* mWindow, Ogre::Camera* mCamera, OgreBites::InputContext& mInputContext,
                   OgreBites::SdkTrayManager* mTrayMgr, OgreBites::SdkCameraMan* mCameraMan, OgreBites::ParamsPanel* mDetailsPanel, bool& mShutDown);
    ~Listeners();
    void createFrameListener();

private:
    Ogre::Root*                mRoot;
    Ogre::Camera*              mCamera;
    Ogre::RenderWindow*        mWindow;
    OgreBites::InputContext&   mInputContext;
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::SdkCameraMan*   mCameraMan;
    OgreBites::ParamsPanel*    mDetailsPanel;
    bool&                      mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*        mMouse;
    OIS::Keyboard*     mKeyboard;

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
    void windowClosed  (Ogre::RenderWindow* rw); // Unattach OIS before window shutdown (very important under Linux)
};

#endif // LISTENERS_HPP_INCLUDED
