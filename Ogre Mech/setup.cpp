#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreTextureManager.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>

#include "setup.hpp"
#include "listeners.hpp"
#include "main.hpp"

void go()
{
    Ogre::Root*          mRoot          (0);
    Ogre::Camera*        mCamera        (0);
    Ogre::SceneManager*  mSceneMgr      (0);
    Ogre::RenderWindow*  mWindow        (0);
    Ogre::Viewport*      mViewport      (0);
    Ogre::String         mResourcesCfg  (Ogre::BLANKSTRING);
    Ogre::OverlaySystem* mOverlaySystem (0);

    OgreBites::InputContext     mInputContext;
    OgreBites::SdkTrayManager*	mTrayMgr       (0);
    OgreBites::SdkCameraMan*    mCameraMan     (0); // Basic camera controller
    OgreBites::ParamsPanel*     mDetailsPanel  (0); // Sample details panel

    bool mShutDown         (false);

    mRoot = new Ogre::Root (Ogre::BLANKSTRING);

    Ogre::GLPlugin* mGLPlugin = new Ogre::GLPlugin();
    mRoot -> installPlugin (mGLPlugin);
    Ogre::D3D9Plugin* mD3D9Plugin = new Ogre::D3D9Plugin();
    mRoot -> installPlugin (mD3D9Plugin);

    setupResources();

    if (!configure (mRoot, mWindow) )
        return;

    chooseSceneManager (mRoot, mSceneMgr, mOverlaySystem);
    createCamera (mSceneMgr, mCamera, mCameraMan);
    createViewports (mWindow, mViewport, mCamera);

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps (5);

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // Create the scene
    createScene (mSceneMgr);

    Listeners* listeners = new Listeners (mRoot, mWindow, mCamera, mInputContext, mTrayMgr, mCameraMan, mDetailsPanel, mShutDown);
    listeners -> createFrameListener();

    mRoot -> startRendering();

    // Clean up
    destroyScene();

    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;
    if (mOverlaySystem) delete mOverlaySystem;
    if (listeners) delete listeners;
    if (mRoot) delete mRoot;

    if (mGLPlugin) delete mGLPlugin;
}

void setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;

    #ifdef _DEBUG
        cf.load ("resources_d.cfg");
    #else // _DEBUG
        cf.load ("resources.cfg");
    #endif // _DEBUG

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements() )
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;

        for (i = settings -> begin(); i != settings -> end(); ++i)
        {
            typeName = i -> first;
            archName = i -> second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation (archName, typeName, secName);
        }
    }
}

bool configure (Ogre::Root* mRoot, Ogre::RenderWindow*& mWindow)
{
    if (mRoot -> showConfigDialog() )
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot -> initialise (true, "Ogre Render Window");

        return true;
    }

    else
        return false;
}

void chooseSceneManager (Ogre::Root* mRoot, Ogre::SceneManager*& mSceneMgr, Ogre::OverlaySystem* mOverlaySystem)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot -> createSceneManager (Ogre::ST_GENERIC);

    // Initialize the OverlaySystem (changed for Ogre 1.9)
    mOverlaySystem = new Ogre::OverlaySystem();
    mSceneMgr -> addRenderQueueListener (mOverlaySystem);
}

void createCamera (Ogre::SceneManager* mSceneMgr, Ogre::Camera*& mCamera, OgreBites::SdkCameraMan*& mCameraMan)
{
    // Create the camera
    mCamera = mSceneMgr -> createCamera ("PlayerCam");

    // Position it at 500 in Z direction
    mCamera -> setPosition (Ogre::Vector3 (0, 10, 500) );
    // Look back along -Z
    mCamera -> lookAt (Ogre::Vector3 (0, 0, 0) );
    mCamera -> setNearClipDistance (5);

    mCameraMan = new OgreBites::SdkCameraMan (mCamera);   // Create a default camera controller
}

void createViewports (Ogre::RenderWindow* mWindow, Ogre::Viewport* mViewport, Ogre::Camera*& mCamera)
{
    // Create one viewport, entire window
    mViewport = mWindow -> addViewport (mCamera);
    mViewport -> setBackgroundColour (Ogre::ColourValue (0, 0, 0) );

    // Alter the camera aspect ratio to match the viewport
    mCamera -> setAspectRatio (Ogre::Real (mViewport -> getActualWidth() ) / Ogre::Real (mViewport -> getActualHeight() ) );
}

void createResourceListener()
{
    //Nothing yet
}
