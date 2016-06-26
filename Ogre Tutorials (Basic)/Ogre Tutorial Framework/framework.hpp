#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>

#ifndef FRAMEWORK_HPP_INCLUDED
#define FRAMEWORK_HPP_INCLUDED

class Framework : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public OgreBites::SdkTrayListener
{
public:
    Framework();
    virtual ~Framework();
    virtual void go();

protected:
    //Ogre
    Ogre::Root*          mRoot;
    Ogre::RenderWindow*  mWindow;
    Ogre::SceneManager*  mSceneMgr;
    Ogre::OverlaySystem* mOverlaySystem;
    Ogre::Camera*        mCamera;
    Ogre::Viewport*      mViewport;
    bool                 mShutDown;

    //Static Plugins
    Ogre::GLPlugin*   mGLPlugin;
    Ogre::D3D9Plugin* mD3D9Plugin;

    //OgreBites
    OgreBites::SdkCameraMan*   mCameraMan;
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

    virtual bool configure();
    virtual void chooseSceneManager();
    virtual void createCamera();
    virtual void createViewports();
    virtual void setupResources();

    virtual void createResourceListener();
    virtual void createFrameListener();
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);

    virtual bool keyPressed (const OIS::KeyEvent& arg);
    virtual bool keyReleased (const OIS::KeyEvent& arg);
    virtual bool mouseMoved (const OIS::MouseEvent& arg);
    virtual bool mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    // Ogre::WindowEventListener
    virtual void windowResized (Ogre::RenderWindow* rw); // Adjust mouse clipping area
    virtual void windowClosed  (Ogre::RenderWindow* rw); // Unattach OIS before window shutdown (very important under Linux) and clean up
};

#endif // FRAMEWORK_HPP_INCLUDED
