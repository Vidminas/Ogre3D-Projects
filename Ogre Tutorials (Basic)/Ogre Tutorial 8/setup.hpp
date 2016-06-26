#ifndef SETUP_HPP_INCLUDED
#define SETUP_HPP_INCLUDED

void go();
void setupResources();
bool configure (Ogre::Root* root, Ogre::RenderWindow*& window);
void chooseSceneManager (Ogre::Root* root, Ogre::SceneManager*& primarySceneMgr, Ogre::SceneManager*& secondarySceneMgr, Ogre::OverlaySystem* overlaySystem);
void createCamera (Ogre::SceneManager* primarySceneMgr, Ogre::SceneManager* secondarySceneMgr, Ogre::Camera*& camera, OgreBites::SdkCameraMan*& cameraMan);
void singleViewport (Ogre::RenderWindow* window, Ogre::SceneManager* sceneMgr);
void dualViewport (Ogre::RenderWindow* window, Ogre::SceneManager* primarySceneMgr, Ogre::SceneManager* secondarySceneMgr);
void createResourceListener();

#endif // SETUP_HPP_INCLUDED
