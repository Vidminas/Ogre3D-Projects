#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED

class OgreTutorial : public Framework
{
public:
    OgreTutorial();
    ~OgreTutorial();

private:
    //Setup
    void injectFrameListener();
    bool injectFrameRendering (const Ogre::FrameEvent& evt);

    //GUI
    OgreBites::ParamsPanel* mInfoPanel;

    //Input
    bool injectKeyPressed (const OIS::KeyEvent& arg);
    bool injectKeyReleased (const OIS::KeyEvent& arg);
    bool injectMouseMoved (const OIS::MouseEvent& arg);
    bool injectMousePressed (const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool injectMouseReleased (const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    //Camera
    static const Ogre::Real mCollisionDistance = 25;
    Ogre::RaySceneQuery*    mRaySceneQuery;

    //Scene
    Ogre::Light* mLight;

    void createScene();
    void destroyScene();

    // Entities
    enum QueryFlags
    {
        ROBOT_MASK = 1 << 0,
        NINJA_MASK = 1 << 1
    };

    bool                 mMode;
    EntityController*    mEntityController;
    bool                 mDetectCollision;

    //Terrain
    Ogre::Terrain*              mTerrain;
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup*         mTerrainGroup;
    bool                        mTerrainsImported;

    void initBlendMaps (Ogre::Terrain* terrain);
    void configureTerrainDefaults();
    void getTerrainImage (bool flipX, bool flipY, Ogre::Image& img);
    void defineTerrain (long x, long y);
    void setupTerrain();
};

#endif // MAIN_HPP_INCLUDED
