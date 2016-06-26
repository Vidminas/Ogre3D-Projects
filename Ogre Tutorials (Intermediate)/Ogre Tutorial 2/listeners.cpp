#include <OgreWindowEventUtilities.h>
#include <OgreLogManager.h>
#include <OgreFrameListener.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <SdkTrays.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "entity.hpp"
#include "robot.hpp"
#include "ninja.hpp"
#include "listeners.hpp"

Listeners::Listeners (Ogre::Root*& root, Ogre::RenderWindow*& window, Ogre::Camera*& camera,
                              Ogre::TerrainGlobalOptions*& terrainGlobals, Ogre::TerrainGroup*& terrainGroup, bool& terrainsImported) :
    mRoot             (root),
    mWindow           (window),
    mCamera           (camera),
    mTrayMgr          (0),
    mDetailsPanel     (0),
    mShutDown         (false),
    mInputManager     (0),
    mMouse            (0),
    mKeyboard         (0),
    mMove             (1000),
    mDirection        (Ogre::Vector3::ZERO),
    mDetectCollision  (true),
    mRobotMode        (true),
    mEntities         (0),
    mCurEntity        (-1),
    mRobots           (0),
    mNinjas           (0),
    mMovableFound     (false),
    mRaySceneQuery    (0),
    mInfoPanel        (0),
    mFastCamera       (0),
    mTerrainGlobals   (terrainGlobals),
    mTerrainGroup     (terrainGroup),
    mTerrainsImported (terrainsImported) {}

Listeners::~Listeners()
{
    mRoot -> getSceneManager ("SceneManager") -> destroyQuery (mRaySceneQuery);

    if (mTrayMgr) delete mTrayMgr;

    for (unsigned short i (0); i < mEntities.size(); i++)
        delete mEntities.at (i);

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener (mWindow, this);
    windowClosed (mWindow);
}

void Listeners::handleCameraCollision()
{
    Ogre::Vector3 camPos = mCamera -> getPosition();
    Ogre::Ray camRay (Ogre::Vector3 (camPos.x, 5000.0, camPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);

    Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (camRay);

    if (result.terrain)
    {
        Ogre::Real terrainHeight = result.position.y;

        if (camPos.y < (terrainHeight + mCollisionDistance) )
            mCamera -> setPosition (camPos.x, terrainHeight + mCollisionDistance, camPos.z);
    }
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

    Ogre::StringVector info;
    info.push_back ("Collision mode (Z to toggle)");
    info.push_back ("Insert Mode (X to toggle)");
    info.push_back ("Robots");
    info.push_back ("Ninjas");
    mInfoPanel = mTrayMgr -> createParamsPanel (OgreBites::TL_TOPLEFT, "InfoPanel", 300, info);
    mInfoPanel -> setParamValue (0, "On");
    mInfoPanel -> setParamValue (1, "Robots");
    mInfoPanel -> setParamValue (2, "0");
    mInfoPanel -> setParamValue (3, "0");

    mFastCamera = mTrayMgr -> createCheckBox (OgreBites::TL_TOP, "FastCamera", "Fast Camera Mode", 200);

    mRaySceneQuery = mRoot -> getSceneManager ("SceneManager") -> createRayQuery (Ogre::Ray() );
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
        if (mFastCamera -> isChecked() )
            mMove = 2500;

        else
            mMove = 1000;

        if (mDirection != Ogre::Vector3::ZERO)
        {
            mCamera -> moveRelative (mDirection * evt.timeSinceLastFrame);

            if (mDirection.y != 0)
                mDirection.y = 0;

            if (mDetectCollision)
                handleCameraCollision();
        }

        for (unsigned short i (0); i < mEntities.size(); i++)
        {
            if (mEntities.at (i) -> isDead (evt.timeSinceLastFrame) )
            {
                delete mEntities.at (i);
                mEntities.erase (mEntities.begin() + i);
            }

            else
            {
                mEntities.at (i) -> animation (evt.timeSinceLastFrame);

                if (mDetectCollision)
                {
                    for (unsigned short j (0); j < mEntities.size(); j++)
                    {
                        if (j == i)
                            continue;

                        mEntities.at (i) -> handleEntityCollision (mEntities.at (j) );
                    }

                    mEntities.at (i) -> handleTerrainCollision (mTerrainGroup);
                }
            }
        }

        mTrayMgr -> adjustTrays();

        if (mDetailsPanel -> isVisible() ) // If details panel is visible, then update its contents
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

    return true;
}

bool Listeners::keyPressed (const OIS::KeyEvent& arg)
{
    if (mTrayMgr -> isDialogVisible() )
        return true; // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F) // toggle visibility of advanced frame stats
        mTrayMgr -> toggleAdvancedFrameStats();

    else if (arg.key == OIS::KC_Z)
    {
        mDetectCollision = !mDetectCollision;

        if (mDetectCollision)
            mInfoPanel -> setParamValue (0, "On");

        else
            mInfoPanel -> setParamValue (0, "Off");
    }

    else if (arg.key == OIS::KC_X)
    {
        mRobotMode = !mRobotMode;

        if (mRobotMode)
            mInfoPanel -> setParamValue (1, "Robots");

        else
            mInfoPanel -> setParamValue (1, "Ninjas");
    }

    else if (arg.key == OIS::KC_SPACE)
    {
        for (unsigned short i (0); i < mEntities.size(); i++)
            mEntities.at (i) -> die();

        mRobots = 0;
        mNinjas = 0;
        mInfoPanel -> setParamValue (2, "0");
        mInfoPanel -> setParamValue (3, "0");
    }

    else if (arg.key == OIS::KC_G) // toggle visibility of even rarer debugging details
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

    else if (arg.key == OIS::KC_T) // cycle polygon rendering mode
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

    else if (arg.key == OIS::KC_UP or arg.key == OIS::KC_W)
        mDirection.z -= mMove;

    else if (arg.key == OIS::KC_DOWN or arg.key == OIS::KC_S)
        mDirection.z += mMove;

    else if (arg.key == OIS::KC_LEFT or arg.key == OIS::KC_A)
        mDirection.x -= mMove;

    else if (arg.key == OIS::KC_RIGHT or arg.key == OIS::KC_D)
        mDirection.x += mMove;

    return true;
}

bool Listeners::keyReleased (const OIS::KeyEvent& arg)
{
    if (arg.key == OIS::KC_UP or arg.key == OIS::KC_W)
    {
        if (mDirection.z < 0)
            mDirection.z = 0;
    }

    else if (arg.key == OIS::KC_DOWN or arg.key == OIS::KC_S)
    {
        if (mDirection.z > 0)
            mDirection.z = 0;
    }

    else if (arg.key == OIS::KC_LEFT or arg.key == OIS::KC_A)
    {
        if (mDirection.x < 0)
            mDirection.x = 0;
    }

    else if (arg.key == OIS::KC_RIGHT or arg.key == OIS::KC_D)
    {
        if (mDirection.x > 0)
            mDirection.x = 0;
    }

    return true;
}

bool Listeners::mouseMoved (const OIS::MouseEvent& arg)
{
    if (mTrayMgr -> injectPointerMove (arg) )
        return true;

    if (arg.state.buttonDown (OIS::MB_Middle) )
    {
        mCamera -> yaw (Ogre::Degree (-mRotate * arg.state.X.rel) );
        mCamera -> pitch (Ogre::Degree (-mRotate * arg.state.Y.rel) );
    }

    else if (arg.state.buttonDown (OIS::MB_Left) && !mEntities.empty() )
    {
        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);
        Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

        if (result.terrain)
        {
            for (unsigned short i (0); i < mEntities.size(); i++)
            {
                if (mEntities.at (i) -> isSelected() )
                    mEntities.at (i) -> getSceneNode() -> setPosition (result.position);
            }
        }
    }

    if (arg.state.Z.rel > 0)
        mDirection.y += arg.state.Z.rel + mMove;

    else if (arg.state.Z.rel < 0)
        mDirection.y += arg.state.Z.rel - mMove;

    return true;
}

bool Listeners::mousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerDown (arg, id) )
        return true;

    if (id == OIS::MB_Middle)
        mTrayMgr -> hideCursor();

    else if (id == OIS::MB_Left)
    {
        if (!mKeyboard -> isModifierDown (OIS::Keyboard::Ctrl) )
        {
            for (unsigned short i (0); i < mEntities.size(); i++)
            {
                if (mEntities.at (i) -> isSelected() )
                    mEntities.at (i) -> getSceneNode() -> showBoundingBox (false);
            }
        }

        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);

        mRaySceneQuery -> setRay (mouseRay);
        mRaySceneQuery -> setSortByDistance (true);
        mRaySceneQuery -> setQueryMask (mRobotMode ? ROBOT_MASK : NINJA_MASK);
        Ogre::RaySceneQueryResult& result = mRaySceneQuery -> execute();
        mMovableFound = false;

        for (Ogre::RaySceneQueryResult::iterator it = result.begin(); it != result.end(); it++)
        {
            mMovableFound = it -> movable && it -> movable -> getName() != "" && it -> movable -> getName() != "PlayerCam";

            if (mMovableFound)
            {
                it -> movable -> getParentSceneNode() -> showBoundingBox (true);
/*
                for (unsigned int x (0); x < mEntities.size(); x++)
                {
                    if (it -> movable -> getParentSceneNode() == mEntities.at (x) -> getSceneNode() )
                    {
                        mCurEntity = x;
                        break;
                    }
                }
*/
            }
        }

        if (!mMovableFound)
        {
            Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

            if (result.terrain)
            {
                if (mRobotMode)
                {
                    mEntities.push_back (new Robot (mRoot -> getSceneManager ("SceneManager"), result.position, 200) );
                    mEntities.back() -> getSceneNode() -> getAttachedObject (0) -> setQueryFlags (ROBOT_MASK);
                    mRobots++;
                    std::ostringstream numRobots;
                    numRobots << mRobots;
                    mInfoPanel -> setParamValue (2, numRobots.str() );
                }

                else
                {
                    mEntities.push_back (new Ninja (mRoot -> getSceneManager ("SceneManager"), result.position, 200, mNinjas) );
                    mEntities.back() -> getSceneNode() -> getAttachedObject (0) -> setQueryFlags (NINJA_MASK);
                    mNinjas++;
                    std::ostringstream numNinjas;
                    numNinjas << mNinjas;
                    mInfoPanel -> setParamValue (3, numNinjas.str() );
                }

                mEntities.back() -> getSceneNode() -> showBoundingBox (true);
            }
        }
    }

    else if (id == OIS::MB_Right)
    {
        Ogre::Ray mouseRay = mTrayMgr -> getCursorRay (mCamera);
        Ogre::TerrainGroup::RayResult result = mTerrainGroup -> rayIntersects (mouseRay);

        if (result.terrain)
        {
            for (unsigned short i (0); i < mEntities.size(); i++)
            {
                if (mEntities.at (i) -> isSelected() )
                    mEntities.at (i) -> addDestination (result.position);
            }
        }

    }

    return true;
}

bool Listeners::mouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (mTrayMgr -> injectPointerUp (arg, id) )
        return true;

    if (id == OIS::MB_Middle)
        mTrayMgr -> showCursor();

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
