#include <OgreConfigFile.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>

#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OgreGLPlugin.h>
#include <OgreD3D9Plugin.h>

#include "framework.hpp"

Framework::Framework() :
    //Ogre
    mRoot          (0),
    mWindow        (0),
    mSceneMgr      (0),
    mOverlaySystem (0),
    mCamera        (0),
    mViewport      (0),
    mShutDown      (false),

    //Camera handler
    mCamDir (Ogre::Vector3::ZERO),
    mFastCam  (false),

    //Static Plugins
    mGLPlugin   (0),
    mD3D9Plugin (0),

    //OgreBites
    mTrayMgr      (0),
    mDetailsPanel (0),

    //OIS Input devices
    mInputManager (0),
    mMouse        (0),
    mKeyboard     (0)
{
    // Nothing needed yet
}

Framework::~Framework()
{
    if (mTrayMgr)       delete mTrayMgr;
    if (mOverlaySystem) delete mOverlaySystem;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener (mWindow, this);
    windowClosed (mWindow);

    if (mRoot)       delete mRoot;
    if (mGLPlugin)   delete mGLPlugin;
    if (mD3D9Plugin) delete mD3D9Plugin;
}

void Framework::start()
{
    if (setup() )
    {
        mRoot -> startRendering();
        destroyScene();
    }
}

bool Framework::setup()
{
    mRoot       = new Ogre::Root (Ogre::BLANKSTRING);
    mGLPlugin   = new Ogre::GLPlugin();
    mD3D9Plugin = new Ogre::D3D9Plugin();
    mRoot -> installPlugin (mGLPlugin);
    mRoot -> installPlugin (mD3D9Plugin);

    configure();
    createSceneManager();
    createCamera();
    createViewports();
    setupResources();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps (5);

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // Create the scene
    createScene();

    createFrameListener();
    mRoot -> addFrameListener (this);

    return true;
}

bool Framework::configure()
{
    // If returned true, user clicked OK so initialise.
    // Here we choose to let the system create a default rendering window by passing 'true'.
    if (mRoot -> showConfigDialog() )
    {
        mWindow = mRoot -> initialise (true, "Ogre Window");
        return true;
    }

    else
        return false;
}

void Framework::createSceneManager()
{
    // Use the terrain scene manager.
	mSceneMgr = mRoot -> createSceneManager (Ogre::ST_EXTERIOR_CLOSE);

	// Initialize the OverlaySystem (changed for Ogre 1.9)
    mOverlaySystem = new Ogre::OverlaySystem();
    mSceneMgr -> addRenderQueueListener (mOverlaySystem);
}

void Framework::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr -> createCamera ("PlayerCam");

    mCamera -> setPosition (0, 500, 0);
    mCamera -> pitch (Ogre::Degree (-30) );
    mCamera -> yaw (Ogre::Degree (-45) );
    mCamera -> setNearClipDistance (0.1);

    if (mRoot -> getRenderSystem() -> getCapabilities() -> hasCapability (Ogre::RSC_INFINITE_FAR_PLANE) )
        mCamera -> setFarClipDistance (0); // enable infinite far clip distance if we can

    else
        mCamera -> setFarClipDistance (50000);
}

void Framework::createViewports()
{
    // Create one viewport, entire window
    mViewport = mWindow -> addViewport (mCamera);
    mViewport -> setBackgroundColour (Ogre::ColourValue (0, 0, 0) );

    // Alter the camera aspect ratio to match the viewport
    mCamera -> setAspectRatio (Ogre::Real (mViewport -> getActualWidth() ) / Ogre::Real (mViewport -> getActualHeight() ) );
}

void Framework::setupResources()
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

void Framework::createResourceListener()
{
    //Nothing yet
}

void Framework::createFrameListener()
{
    Ogre::LogManager::getSingletonPtr() -> logMessage ("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow -> getCustomAttribute ("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert (std::make_pair (std::string("WINDOW"), windowHndStr.str() ) );

    mInputManager = OIS::InputManager::createInputSystem (pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager -> createInputObject (OIS::OISKeyboard, true) );
    mMouse = static_cast<OIS::Mouse*>(mInputManager -> createInputObject (OIS::OISMouse, true) );

    mMouse -> setEventCallback (this);
    mKeyboard -> setEventCallback (this);

    // Set initial mouse clipping size
    windowResized (mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener (mWindow, this);

    mInputContext.mKeyboard = mKeyboard;
    mInputContext.mMouse = mMouse;
    mTrayMgr = new OgreBites::SdkTrayManager ("InterfaceName", mWindow, mInputContext, this);
    mTrayMgr -> showFrameStats (OgreBites::TL_BOTTOMLEFT);
    mTrayMgr -> showLogo (OgreBites::TL_BOTTOMRIGHT);

    // Create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back ("cam.pX");
    items.push_back ("cam.pY");
    items.push_back ("cam.pZ");
    items.push_back ("");
    items.push_back ("cam.oW");
    items.push_back ("cam.oX");
    items.push_back ("cam.oY");
    items.push_back ("cam.oZ");
    items.push_back ("");
    items.push_back ("Filtering");
    items.push_back ("Poly Mode");

    mDetailsPanel = mTrayMgr -> createParamsPanel (OgreBites::TL_NONE, "DetailsPanel", 200, items);
    mDetailsPanel -> setParamValue (9, "Bilinear");
    mDetailsPanel -> setParamValue (10, "Solid");
    mDetailsPanel -> hide();

    injectFrameListener();
}

bool Framework::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    if (mWindow -> isClosed() || mShutDown)
        return false;

    // Need to capture/update each device
    mKeyboard -> capture();
    mMouse -> capture();

    mTrayMgr -> frameRenderingQueued (evt);

    // If dialog isn't up, then update the camera
    if (!mTrayMgr -> isDialogVisible() )
    {
        // Move the camera
        if (mCamDir != Ogre::Vector3::ZERO)
        {
            mCamera -> moveRelative (mCamDir * (mFastCam ? mCamSpeed * 3 : mCamSpeed) * evt.timeSinceLastFrame);

            if (mCamDir.y != 0)
                mCamDir.y = 0;
        }

        // If details panel is visible, then update its contents
        if (mDetailsPanel -> isVisible() )
        {
            mDetailsPanel -> setParamValue (0, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().x) );
            mDetailsPanel -> setParamValue (1, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().y) );
            mDetailsPanel -> setParamValue (2, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().z) );
            mDetailsPanel -> setParamValue (4, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().w) );
            mDetailsPanel -> setParamValue (5, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().x) );
            mDetailsPanel -> setParamValue (6, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().y) );
            mDetailsPanel -> setParamValue (7, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().z) );
        }

        return injectFrameRendering (evt);
    }

    return true;
}

bool Framework::keyPressed (const OIS::KeyEvent& arg)
{
    // don't process any more keys if dialog is up
    if (mTrayMgr -> isDialogVisible() )
        return true;

    switch (arg.key)
    {
        // close window when esc is pressed
        case OIS::KC_ESCAPE:
            mShutDown = true;
            break;

        // toggle visibility of advanced frame stats
        case OIS::KC_F:
            mTrayMgr -> toggleAdvancedFrameStats();
            break;

        // toggle visibility of even rarer debugging details
        case OIS::KC_G:
            if (mDetailsPanel -> getTrayLocation() == OgreBites::TL_NONE)
            {
                mTrayMgr -> moveWidgetToTray (mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
                mDetailsPanel -> show();
            }

            else
            {
                mTrayMgr -> removeWidgetFromTray (mDetailsPanel);
                mDetailsPanel -> hide();
            }
            break;

        // cycle polygon rendering mode
        case OIS::KC_T:
            if (mDetailsPanel -> getParamValue(9).asUTF8() == "Bilinear")
            {
                mDetailsPanel -> setParamValue (9, "Trilinear");
                Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (Ogre::TFO_TRILINEAR);
                Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (1);
            }

            else if (mDetailsPanel -> getParamValue(9).asUTF8() == "Trilinear")
            {
                mDetailsPanel -> setParamValue (9, "Anisotropic");
                Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (Ogre::TFO_ANISOTROPIC);
                Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (8);
            }

            else if (mDetailsPanel -> getParamValue(9).asUTF8() == "Anisotropic")
            {
                mDetailsPanel -> setParamValue (9, "None");
                Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (Ogre::TFO_NONE);
                Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (1);
            }

            else
            {
                mDetailsPanel -> setParamValue (9, "Bilinear");
                Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (Ogre::TFO_BILINEAR);
                Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (1);
            }
            break;

        // cycle polygon rendering mode
        case OIS::KC_R:
            if (mCamera -> getPolygonMode() == Ogre::PM_SOLID)
            {
                mDetailsPanel -> setParamValue (10, "Wireframe");
                mCamera -> setPolygonMode (Ogre::PM_WIREFRAME);
            }

            else if (mCamera -> getPolygonMode() == Ogre::PM_WIREFRAME)
            {
                mDetailsPanel -> setParamValue (10, "Points");
                mCamera -> setPolygonMode (Ogre::PM_POINTS);
            }

            else
            {
                mDetailsPanel -> setParamValue (10, "Solid");
                mCamera -> setPolygonMode (Ogre::PM_SOLID);
            }
            break;

        // refresh all textures
        case OIS::KC_F5:
            Ogre::TextureManager::getSingleton().reloadAll();
            break;

        // take a screenshot
        case OIS::KC_SYSRQ:
            mWindow -> writeContentsToTimestampedFile ("screenshot", ".jpg");
            break;

        case OIS::KC_LSHIFT:
            mFastCam = true;
            break;

        case OIS::KC_UP:
        case OIS::KC_W:
            mCamDir.z--;
            break;

        case OIS::KC_DOWN:
        case OIS::KC_S:
            mCamDir.z++;
            break;

        case OIS::KC_LEFT:
        case OIS::KC_A:
            mCamDir.x--;
            break;

        case OIS::KC_RIGHT:
        case OIS::KC_D:
            mCamDir.x++;
            break;

        default:
            break;
    }

    return injectKeyPressed (arg);
}

bool Framework::keyReleased (const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
        case OIS::KC_LSHIFT:
            mFastCam = false;
            break;

        case OIS::KC_UP:
        case OIS::KC_W:
            if (mCamDir.z < 0)
                mCamDir.z = 0;
            break;

        case OIS::KC_DOWN:
        case OIS::KC_S:
            if (mCamDir.z > 0)
                mCamDir.z = 0;
            break;

        case OIS::KC_LEFT:
        case OIS::KC_A:
            if (mCamDir.x < 0)
                mCamDir.x = 0;
            break;

        case OIS::KC_RIGHT:
        case OIS::KC_D:
            if (mCamDir.x > 0)
                mCamDir.x = 0;
            break;

        default:
            break;
    }

    return injectKeyReleased (arg);
}

bool Framework::mouseMoved (const OIS::MouseEvent& arg)
{
    if (mTrayMgr -> injectPointerMove (arg) )
        return true;

    if (arg.state.buttonDown (OIS::MB_Middle) )
    {
        mCamera -> yaw (Ogre::Degree (-mRotateSpeed * arg.state.X.rel) );
        mCamera -> pitch (Ogre::Degree (-mRotateSpeed * arg.state.Y.rel) );
    }

    if (arg.state.Z.rel > 0)
        mCamDir.y += arg.state.Z.rel;

    else if (arg.state.Z.rel < 0)
        mCamDir.y += arg.state.Z.rel;

    return injectMouseMoved (arg);
}

bool Framework::mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerDown (arg, id) )
        return true;

    if (id == OIS::MB_Middle)
        mTrayMgr -> hideCursor();

    return injectMousePressed (arg, id);
}

bool Framework::mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerUp (arg, id) )
        return true;

    if (id == OIS::MB_Middle)
        mTrayMgr -> showCursor();

    return injectMouseReleased (arg, id);
}

void Framework::windowResized (Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw -> getMetrics (width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse -> getMouseState();
    ms.width = width;
    ms.height = height;
}

void Framework::windowClosed (Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS (the main window in these demos)
    if (rw == mWindow)
    {
        if (mInputManager)
        {
            mInputManager -> destroyInputObject (mMouse);
            mInputManager -> destroyInputObject (mKeyboard);

            OIS::InputManager::destroyInputSystem (mInputManager);
            mInputManager = 0;
        }
    }
}
