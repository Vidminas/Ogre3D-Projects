/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
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

#include "BaseApplication.h"
#include <iostream>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <macUtils.h>
#endif

//---------------------------------------------------------------------------
BaseApplication::BaseApplication() :
    mRoot (0),
    mCamera (0),
    mSceneMgr (0),
    mWindow (0),
    mResourcesCfg (Ogre::BLANKSTRING),
    mPluginsCfg (Ogre::BLANKSTRING),
    mOverlaySystem (0),
    mTrayMgr (0),
    mCameraMan (0),
    mDetailsPanel (0),
    mCursorWasVisible (false),
    mShutDown (false),
    mInputManager (0),
    mMouse (0),
    mKeyboard (0)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mResourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
    mResourcePath = "";
#endif
}

//---------------------------------------------------------------------------
BaseApplication::~BaseApplication()
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;
    if (mOverlaySystem) delete mOverlaySystem;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener (mWindow, this);
    windowClosed (mWindow);
    delete mRoot;

    #ifdef OGRE_STATIC_LIB
        mStaticPluginLoader.unload();
    #endif // OGRE_STATIC_LIB
}

//---------------------------------------------------------------------------
bool BaseApplication::configure()
{
    if (mRoot -> restoreConfig() || mRoot -> showConfigDialog() )
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot -> initialise (true, "TutorialApplication Render Window");

        return true;
    }

    else
        return false;
}
//---------------------------------------------------------------------------
void BaseApplication::chooseSceneManager()
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot -> createSceneManager (Ogre::ST_GENERIC);

    // Initialize the OverlaySystem (changed for Ogre 1.9)
    mOverlaySystem = new Ogre::OverlaySystem();
    mSceneMgr -> addRenderQueueListener (mOverlaySystem);
}
//---------------------------------------------------------------------------
void BaseApplication::createCamera()
{
    // Create the camera
    mCamera = mSceneMgr -> createCamera ("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition (Ogre::Vector3 (0,0,80) );
    // Look back along -Z
    mCamera->lookAt (Ogre::Vector3 (0,0,-300) );
    mCamera->setNearClipDistance (5);

    mCameraMan = new OgreBites::SdkCameraMan (mCamera);   // Create a default camera controller
}
//---------------------------------------------------------------------------
void BaseApplication::createFrameListener()
{
    Ogre::LogManager::getSingletonPtr()->logMessage ("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute ("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert (std::make_pair (std::string("WINDOW"), windowHndStr.str() ) );

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject (OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject (OIS::OISMouse, true));

    mMouse->setEventCallback (this);
    mKeyboard->setEventCallback (this);

    // Set initial mouse clipping size
    windowResized (mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener (mWindow, this);

    mInputContext.mKeyboard = mKeyboard;
    mInputContext.mMouse = mMouse;
    mTrayMgr = new OgreBites::SdkTrayManager ("InterfaceName", mWindow, mInputContext, this);
    mTrayMgr -> showFrameStats (OgreBites::TL_BOTTOMLEFT);
    mTrayMgr -> showLogo (OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr -> hideCursor();

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

    mDetailsPanel = mTrayMgr->createParamsPanel (OgreBites::TL_NONE, "DetailsPanel", 200, items);
    mDetailsPanel -> setParamValue (9, "Bilinear");
    mDetailsPanel -> setParamValue (10, "Solid");
    mDetailsPanel -> hide();

    mRoot -> addFrameListener (this);
}

void BaseApplication::createScene()
{
    // Nothing yet
}
//---------------------------------------------------------------------------
void BaseApplication::destroyScene()
{
    // Nothing yet
}
//---------------------------------------------------------------------------
void BaseApplication::createViewports()
{
    // Create one viewport, entire window
    mViewport = mWindow -> addViewport (mCamera);
    mViewport -> setBackgroundColour (Ogre::ColourValue (0, 0, 0) );

    // Alter the camera aspect ratio to match the viewport
    mCamera -> setAspectRatio (Ogre::Real (mViewport -> getActualWidth() ) / Ogre::Real (mViewport -> getActualHeight() ) );
}
//---------------------------------------------------------------------------
void BaseApplication::setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load (mResourcesCfg);

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

            #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                // OS X does not set the working directory relative to the app.
                // In order to make things portable on OS X we need to provide
                // the loading with it's own bundle path location.
                if (!Ogre::StringUtil::startsWith (archName, "/", false) ) // only adjust relative directories
                    archName = Ogre::String (Ogre::macBundlePath() + "/" + archName);
            #endif

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation (archName, typeName, secName);
        }
    }
}
//---------------------------------------------------------------------------
void BaseApplication::createResourceListener()
{
    //Nothing yet
}
//---------------------------------------------------------------------------
void BaseApplication::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//---------------------------------------------------------------------------
void BaseApplication::go()
{
#ifndef OGRE_STATIC_LIB
    #ifdef _DEBUG
        mPluginsCfg = mResourcePath + "plugins_d.cfg";
    #else
        mPluginsCfg = mResourcePath + "plugins.cfg";
    #endif
#endif

#ifdef _DEBUG
    mResourcesCfg = mResourcePath + "resources_d.cfg";
#else
    mResourcesCfg = mResourcePath + "resources.cfg";
#endif

    if (!setup() )
        return;

    mRoot -> startRendering();

    // Clean up
    destroyScene();
}
//---------------------------------------------------------------------------
bool BaseApplication::setup()
{
    mRoot = new Ogre::Root (mPluginsCfg);
    #ifdef OGRE_STATIC_LIB
        mStaticPluginLoader.load();
    #endif
    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    if (Ogre::RTShader::ShaderGenerator::initialize() )
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

        // Set shader cache path.
        mShaderGenerator -> setShaderCachePath ("Media/RTShaderLib/cache");

        // Set the scene manager.
        mShaderGenerator -> addSceneManager (mSceneMgr);

        // Add a specialized sub-render (per-pixel lighting) state to the default scheme render state
        Ogre::RTShader::RenderState* pMainRenderState =
            mShaderGenerator -> createOrRetrieveRenderState (Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
        pMainRenderState -> reset();

        // Create shader based technique from the default technique of the given material.
        mShaderGenerator -> createShaderBasedTechnique ("Media/Materials/Scripts/ogre.material", Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

        mViewport -> setMaterialScheme (Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

        //mTechniqueListener = new RTSSDefaultTechniqueListener (mShaderGenerator); I don't really know what to do with these lines
        //Ogre::MaterialManager::getSingleton().addListener (mTechniqueListener);
    }

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps (5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//---------------------------------------------------------------------------
Ogre::Technique* BaseApplication::handleSchemeNotFound
(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
{
    /*** This is the hook point where shader based technique will be created.
    It will be called whenever the material manager won't find appropriate technique
    that satisfy the target scheme name. If the scheme name is out target RT Shader System
    scheme name we will try to create shader generated technique for it. ***/
    Ogre::Technique* generatedTech = NULL;

    // Case this is the default shader generator scheme.
    if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
    {
        // Create shader generated technique for this material.
        if (mShaderGenerator -> createShaderBasedTechnique (originalMaterial -> getName(), Ogre::MaterialManager::DEFAULT_SCHEME_NAME, schemeName) )
        {
            // Force creating the shaders for the generated technique.
            mShaderGenerator -> validateMaterial (schemeName, originalMaterial -> getName() );

            // Grab the generated technique.
            Ogre::Material::TechniqueIterator itTech = originalMaterial -> getTechniqueIterator();

            while (itTech.hasMoreElements() )
            {
                Ogre::Technique* curTech = itTech.getNext();

                if (curTech -> getSchemeName() == schemeName)
                {
                    generatedTech = curTech;
                    break;
                }
            }
        }
    }

    return generatedTech;
}
//---------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    if (mWindow -> isClosed() )
        return false;

    if (mShutDown)
        return false;

    // Need to capture/update each device
    mKeyboard -> capture();
    mMouse -> capture();

    mTrayMgr -> frameRenderingQueued (evt);

    if (!mTrayMgr -> isDialogVisible() )
    {
        mCameraMan->frameRenderingQueued (evt);   // If dialog isn't up, then update the camera

        if (mDetailsPanel -> isVisible() )          // If details panel is visible, then update its contents
        {
            mDetailsPanel -> setParamValue (0, Ogre::StringConverter::toString (mCamera->getDerivedPosition().x) );
            mDetailsPanel -> setParamValue (1, Ogre::StringConverter::toString (mCamera->getDerivedPosition().y) );
            mDetailsPanel -> setParamValue (2, Ogre::StringConverter::toString (mCamera->getDerivedPosition().z) );
            mDetailsPanel -> setParamValue (4, Ogre::StringConverter::toString (mCamera->getDerivedOrientation().w) );
            mDetailsPanel -> setParamValue (5, Ogre::StringConverter::toString (mCamera->getDerivedOrientation().x) );
            mDetailsPanel -> setParamValue (6, Ogre::StringConverter::toString (mCamera->getDerivedOrientation().y) );
            mDetailsPanel -> setParamValue (7, Ogre::StringConverter::toString (mCamera->getDerivedOrientation().z) );
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::keyPressed (const OIS::KeyEvent& arg)
{
    if (mTrayMgr -> isDialogVisible() )
        return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
        mTrayMgr -> toggleAdvancedFrameStats();

    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
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
    }

    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel -> getParamValue(9).asUTF8()[0])
        {
            case 'B':
                newVal = "Trilinear";
                tfo = Ogre::TFO_TRILINEAR;
                aniso = 1;
                break;

            case 'T':
                newVal = "Anisotropic";
                tfo = Ogre::TFO_ANISOTROPIC;
                aniso = 8;
                break;

            case 'A':
                newVal = "None";
                tfo = Ogre::TFO_NONE;
                aniso = 1;
                break;

            default:
                newVal = "Bilinear";
                tfo = Ogre::TFO_BILINEAR;
                aniso = 1;
                break;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering (tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy (aniso);
        mDetailsPanel -> setParamValue (9, newVal);
    }

    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera -> getPolygonMode() )
        {
            case Ogre::PM_SOLID:
                newVal = "Wireframe";
                pm = Ogre::PM_WIREFRAME;
                break;

            case Ogre::PM_WIREFRAME:
                newVal = "Points";
                pm = Ogre::PM_POINTS;
                break;

            default:
                newVal = "Solid";
                pm = Ogre::PM_SOLID;
                break;
        }

        mCamera -> setPolygonMode (pm);
        mDetailsPanel -> setParamValue (10, newVal);
    }

    else if (arg.key == OIS::KC_F5)   // refresh all textures
        Ogre::TextureManager::getSingleton().reloadAll();

    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
        mWindow -> writeContentsToTimestampedFile ("screenshot", ".jpg");

    else if (arg.key == OIS::KC_ESCAPE)
        mShutDown = true;

    mCameraMan -> injectKeyDown (arg);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::keyReleased (const OIS::KeyEvent& arg)
{
    mCameraMan -> injectKeyUp (arg);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mouseMoved (const OIS::MouseEvent& arg)
{
    if (mTrayMgr -> injectPointerMove (arg) ) return true;
    mCameraMan -> injectPointerMove (arg);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerDown (arg, id) ) return true;
    mCameraMan -> injectPointerDown (arg, id);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerUp (arg, id) ) return true;
    mCameraMan -> injectPointerUp (arg, id);
    return true;
}
//---------------------------------------------------------------------------
// Adjust mouse clipping area
void BaseApplication::windowResized (Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics (width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse -> getMouseState();
    ms.width = width;
    ms.height = height;
}
//---------------------------------------------------------------------------
// Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed (Ogre::RenderWindow* rw)
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
//---------------------------------------------------------------------------
