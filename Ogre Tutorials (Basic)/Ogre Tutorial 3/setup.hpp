#ifndef SETUP_HPP_INCLUDED
#define SETUP_HPP_INCLUDED

void go();
void setupResources();
bool configure (Ogre::Root* root, Ogre::RenderWindow*& window);
void chooseSceneManager (Ogre::Root* root, Ogre::SceneManager*& sceneMgr, Ogre::OverlaySystem* overlaySystem);
void createCamera (Ogre::Root* root, Ogre::SceneManager* sceneMgr, Ogre::Camera*& camera, OgreBites::SdkCameraMan*& cameraMan);
void createViewports (Ogre::RenderWindow* window, Ogre::Viewport*& viewport, Ogre::Camera*& camera);
void createResourceListener();

#endif // SETUP_HPP_INCLUDED
