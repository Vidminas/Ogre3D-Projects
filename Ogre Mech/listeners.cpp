#include <OgreWindowEventUtilities.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "listeners.hpp"

Listeners::Listeners (Ogre::Root* mRoot, Ogre::RenderWindow* mWindow, Ogre::Camera* mCamera, OgreBites::InputContext& mInputContext,
            OgreBites::SdkTrayManager* mTrayMgr, OgreBites::SdkCameraMan* mCameraMan, OgreBites::ParamsPanel* mDetailsPanel, bool& mShutDown) :
    mRoot (mRoot),
    mCamera (mCamera),
    mWindow (mWindow),
    mInputContext (mInputContext),
    mTrayMgr (mTrayMgr),
    mCameraMan (mCameraMan),
    mDetailsPanel (mDetailsPanel),
    mShutDown (mShutDown),
    mInputManager (0),
    mMouse (0),
    mKeyboard (0) {}

Listeners::~Listeners()
{
    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener (mWindow, this);
    windowClosed (mWindow);
}

void Listeners::createFrameListener()
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

    mDetailsPanel = mTrayMgr -> createParamsPanel (OgreBites::TL_NONE, "DetailsPanel", 200, items);
    mDetailsPanel -> setParamValue (9, "Bilinear");
    mDetailsPanel -> setParamValue (10, "Solid");
    mDetailsPanel -> hide();

    mRoot -> addFrameListener (this);
}

bool Listeners::frameRenderingQueued (const Ogre::FrameEvent& evt)
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
        mCameraMan -> frameRenderingQueued (evt);   // If dialog isn't up, then update the camera

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

bool Listeners::keyPressed (const OIS::KeyEvent& arg)
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

bool Listeners::keyReleased (const OIS::KeyEvent& arg)
{
    mCameraMan -> injectKeyUp (arg);
    return true;
}

bool Listeners::mouseMoved (const OIS::MouseEvent& arg)
{
    if (mTrayMgr -> injectPointerMove (arg) ) return true;
    mCameraMan -> injectPointerMove (arg);
    return true;
}

bool Listeners::mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerDown (arg, id) ) return true;
    mCameraMan -> injectPointerDown (arg, id);
    return true;
}

bool Listeners::mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerUp (arg, id) ) return true;
    mCameraMan -> injectPointerUp (arg, id);
    return true;
}

void Listeners::windowResized (Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw -> getMetrics (width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse -> getMouseState();
    ms.width = width;
    ms.height = height;
}

void Listeners::windowClosed (Ogre::RenderWindow* rw)
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
