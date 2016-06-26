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
    Ogre::Root*              root          (0);
    Ogre::RenderWindow*      window        (0);
    Ogre::SceneManager*      sceneMgr      (0);
    Ogre::OverlaySystem*     overlaySystem (0);
    Ogre::Camera*            camera        (0);
    Ogre::Viewport*          viewport      (0);

    root = new Ogre::Root (Ogre::BLANKSTRING);

    Ogre::GLPlugin*   myGLPlugin   = new Ogre::GLPlugin();
    Ogre::D3D9Plugin* myD3D9Plugin = new Ogre::D3D9Plugin();
    root -> installPlugin (myGLPlugin);
    root -> installPlugin (myD3D9Plugin);

    setupResources();

    if (!configure (root, window) )
        return;

    chooseSceneManager (root, sceneMgr, overlaySystem);
    createCamera (sceneMgr, camera);
    createViewports (window, viewport, camera);

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps (5);

    // Create any resource listeners (for loading screens)
    createResourceListener();

    // Load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // Create the scene
    createScene (sceneMgr);

    Listeners* listeners = new Listeners (root, window, camera);
    listeners -> createFrameListener();
    root -> addFrameListener (listeners);

    root -> startRendering();

    if (overlaySystem) delete overlaySystem;
    if (listeners)     delete listeners;
    if (root)          delete root;
    if (myGLPlugin)    delete myGLPlugin;
    if (myD3D9Plugin)  delete myD3D9Plugin;
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

bool configure (Ogre::Root* root, Ogre::RenderWindow*& window)
{
    if (root -> showConfigDialog() )
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        window = root -> initialise (true, "Ogre Render Window");

        return true;
    }

    else
        return false;
}

void chooseSceneManager (Ogre::Root* root, Ogre::SceneManager*& sceneMgr, Ogre::OverlaySystem* overlaySystem)
{
    // Get the SceneManager, in this case a generic one
    sceneMgr = root -> createSceneManager (Ogre::ST_GENERIC, "SceneManager");

    // Initialize the OverlaySystem (changed for Ogre 1.9)
    overlaySystem = new Ogre::OverlaySystem();
    sceneMgr -> addRenderQueueListener (overlaySystem);
}

void createCamera (Ogre::SceneManager* sceneMgr, Ogre::Camera*& camera)
{
    // Create the camera
    camera = sceneMgr -> createCamera ("Camera");
}

void createViewports (Ogre::RenderWindow* window, Ogre::Viewport*& viewport, Ogre::Camera*& camera)
{
    // Create one viewport, entire window
    viewport = window -> addViewport (camera);
    viewport -> setBackgroundColour (Ogre::ColourValue (0, 0, 0) );

    // Alter the camera aspect ratio to match the viewport
    camera -> setAspectRatio (Ogre::Real (viewport -> getActualWidth() ) / Ogre::Real (viewport -> getActualHeight() ) );
}

void createResourceListener()
{
    //Nothing yet
}
