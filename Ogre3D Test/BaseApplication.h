/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.10)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    #include <OIS/OISEvents.h>
    #include <OIS/OISInputManager.h>
    #include <OIS/OISKeyboard.h>
    #include <OIS/OISMouse.h>

    #include <OGRE/SdkTrays.h>
    #include <OGRE/SdkCameraMan.h>
#else
    #include <OISEvents.h>
    #include <OISInputManager.h>
    #include <OISKeyboard.h>
    #include <OISMouse.h>

    #include <SdkTrays.h>
    #include <SdkCameraMan.h>
#endif

#ifdef OGRE_STATIC_LIB
    #define OGRE_STATIC_GL3Plus
    #define OGRE_STATIC_BSPSceneManager
    #define OGRE_STATIC_OctreeSceneManager
    #define OGRE_STATIC_ParticleFX
    #include "OgreStaticPluginLoader.h"
#endif

//---------------------------------------------------------------------------

class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, public Ogre::MaterialManager::Listener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
    BaseApplication();
    virtual ~BaseApplication();

    virtual void go();

protected:
    virtual bool setup();
    virtual bool configure();
    virtual void chooseSceneManager();
    virtual void createCamera();
    virtual void createFrameListener();
    virtual void createScene();
    virtual void destroyScene();
    virtual void createViewports();
    virtual void setupResources();
    virtual void createResourceListener();
    virtual void loadResources();
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);

    virtual bool keyPressed (const OIS::KeyEvent& arg);
    virtual bool keyReleased (const OIS::KeyEvent& arg);
    virtual bool mouseMoved (const OIS::MouseEvent& arg);
    virtual bool mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    // Adjust mouse clipping area
    virtual void windowResized (Ogre::RenderWindow* rw);
    // Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed (Ogre::RenderWindow* rw);

    Ogre::Root*                 mRoot;
    Ogre::Camera*               mCamera;
    Ogre::SceneManager*         mSceneMgr;
    Ogre::RenderWindow*         mWindow;
    Ogre::Viewport*             mViewport;
    Ogre::String                mResourcesCfg;
    Ogre::String                mPluginsCfg;

    Ogre::OverlaySystem*        mOverlaySystem;

    // OgreBites
    OgreBites::InputContext     mInputContext;
    OgreBites::SdkTrayManager*	mTrayMgr;
    OgreBites::SdkCameraMan*    mCameraMan;     	// Basic camera controller
    OgreBites::ParamsPanel*     mDetailsPanel;   	// Sample details panel
    bool                        mCursorWasVisible;	// Was cursor visible before dialog appeared?
    bool                        mShutDown;

    //OIS Input devices
    OIS::InputManager*          mInputManager;
    OIS::Mouse*                 mMouse;
    OIS::Keyboard*              mKeyboard;

    // RTSS
    Ogre::RTShader::ShaderGenerator* mShaderGenerator;
    virtual Ogre::Technique* handleSchemeNotFound
    (unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend);

    // Added for Mac compatibility
    Ogre::String                 mResourcePath;

    #ifdef OGRE_STATIC_LIB
        Ogre::StaticPluginLoader mStaticPluginLoader;
    #endif
};

//---------------------------------------------------------------------------
#endif // #ifndef __BaseApplication_h_
//---------------------------------------------------------------------------
