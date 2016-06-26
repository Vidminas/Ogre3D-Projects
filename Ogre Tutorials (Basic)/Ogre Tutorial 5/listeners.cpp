#include <OgreWindowEventUtilities.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include "listeners.hpp"

Listeners::Listeners (Ogre::Root*& root, Ogre::RenderWindow*& window, Ogre::Camera*& camera) :
    mRoot         (root),
    mWindow       (window),
    mCamera       (camera),
    mTrayMgr      (0),
    mDetailsPanel (0),
    mShutDown     (false),
    mInputManager (0),
    mMouse        (0),
    mKeyboard     (0),
    mRotate       (0.13),
    mMove         (250),
    mCamNode      (0),
    mDirection    (Ogre::Vector3::ZERO) {}

Listeners::~Listeners()
{
    if (mTrayMgr) delete mTrayMgr;

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
        if (mDetailsPanel -> isVisible() )          // If details panel is visible, then update its contents
        {
            mDetailsPanel -> setParamValue (0, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().x) );
            mDetailsPanel -> setParamValue (1, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().y) );
            mDetailsPanel -> setParamValue (2, Ogre::StringConverter::toString (mCamera -> getDerivedPosition().z) );
            mDetailsPanel -> setParamValue (4, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().w) );
            mDetailsPanel -> setParamValue (5, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().x) );
            mDetailsPanel -> setParamValue (6, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().y) );
            mDetailsPanel -> setParamValue (7, Ogre::StringConverter::toString (mCamera -> getDerivedOrientation().z) );
        }
    }

    mCamera -> getParentSceneNode() -> translate (mDirection * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

    return true;
}

bool Listeners::keyPressed (const OIS::KeyEvent& arg)
{
    if (mTrayMgr -> isDialogVisible() )
        return true; // don't process any more keys if dialog is up

    switch (arg.key)
    {
        case OIS::KC_ESCAPE:
            mShutDown = true;
            break;

        case OIS::KC_1:
            mCamera -> getParentSceneNode() -> detachObject (mCamera);
            mCamNode = mRoot -> getSceneManager ("SceneManager") -> getSceneNode ("CamNode1");
            mCamNode -> attachObject (mCamera);
            break;

        case OIS::KC_2:
            mCamera -> getParentSceneNode() -> detachObject (mCamera);
            mCamNode = mRoot -> getSceneManager ("SceneManager") -> getSceneNode ("CamNode2");
            mCamNode -> attachObject (mCamera);
            break;

        case OIS::KC_UP:
        case OIS::KC_W:
            mDirection.z = -mMove;
            break;

        case OIS::KC_DOWN:
        case OIS::KC_S:
            mDirection.z = mMove;
            break;

        case OIS::KC_LEFT:
        case OIS::KC_A:
            mDirection.x = -mMove;
            break;

        case OIS::KC_RIGHT:
        case OIS::KC_D:
            mDirection.x = mMove;
            break;

        case OIS::KC_PGDOWN:
        case OIS::KC_E:
            mDirection.y = -mMove;
            break;

        case OIS::KC_PGUP:
        case OIS::KC_Q:
            mDirection.y = mMove;
            break;

        default:
            break;
    }

    return true;
}

bool Listeners::keyReleased (const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
        case OIS::KC_UP:
        case OIS::KC_W:
            if (mDirection.z < 0)
                mDirection.z = 0;
            break;

        case OIS::KC_DOWN:
        case OIS::KC_S:
            if (mDirection.z > 0)
                mDirection.z = 0;
            break;

        case OIS::KC_LEFT:
        case OIS::KC_A:
            if (mDirection.x < 0)
                mDirection.x = 0;
            break;

        case OIS::KC_RIGHT:
        case OIS::KC_D:
            if (mDirection.x > 0)
                mDirection.x = 0;
            break;

        case OIS::KC_PGDOWN:
        case OIS::KC_E:
            if (mDirection.y < 0)
                mDirection.y = 0;
            break;

        case OIS::KC_PGUP:
        case OIS::KC_Q:
            if (mDirection.y > 0)
                mDirection.y = 0;
            break;

        default:
            break;
    }

    return true;
}

bool Listeners::mouseMoved (const OIS::MouseEvent& arg)
{
    if (arg.state.buttonDown (OIS::MB_Right) )
    {
        mCamera -> getParentSceneNode() -> yaw (Ogre::Degree (-mRotate * arg.state.X.rel), Ogre::Node::TS_WORLD);
        mCamera -> getParentSceneNode() -> pitch (Ogre::Degree (-mRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
    }

    return true;
}

bool Listeners::mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    switch (id)
    {
        case OIS::MB_Left:
            static Ogre::Light* light = mRoot -> getSceneManager ("SceneManager") -> getLight ("Light1");
            light -> setVisible (!light->isVisible() );
            break;

        default:
            break;
    }

    return true;
}

bool Listeners::mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
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
