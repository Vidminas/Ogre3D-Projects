#ifndef SETUP_HPP_INCLUDED
#define SETUP_HPP_INCLUDED

void go();
void setupResources();
bool configure (Ogre::Root* mRoot, Ogre::RenderWindow*& mWindow);
void chooseSceneManager (Ogre::Root* mRoot, Ogre::SceneManager*& mSceneMgr, Ogre::OverlaySystem* mOverlaySystem);
void createCamera (Ogre::SceneManager* mSceneMgr, Ogre::Camera*& mCamera, OgreBites::SdkCameraMan*& mCameraMan);
void createViewports (Ogre::RenderWindow* mWindow, Ogre::Viewport* mViewport, Ogre::Camera*& mCamera);
void createResourceListener();

#endif // SETUP_HPP_INCLUDED
