
#include "SimpleFrameListener.h"
////////////////////////////////////////////////////////////////////////// 
// class SimpleFrameListener 
////////////////////////////////////////////////////////////////////////// 
SimpleFrameListener::SimpleFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse) 
{ 
	mKeyboard = keyboard; 
	mMouse = mouse; 
} 
// This gets called before the next frame is beeing rendered.
bool SimpleFrameListener::frameStarted(const FrameEvent& evt) 
{
	//update the input devices
	mKeyboard->capture();
	mMouse->capture();

	//exit if key KC_ESCAPE pressed 
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
		return false; 

	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mCameraNodeYaw->yaw( Radian(-mMouse->getMouseState().X.rel*evt.timeSinceLastFrame) );
		mCameraNodePitch->pitch( Radian(-mMouse->getMouseState().Y.rel*evt.timeSinceLastFrame) );
	}

	float moveZ = 0.0;
	moveZ += (mKeyboard->isKeyDown(OIS::KC_W) || mKeyboard->isKeyDown(OIS::KC_UP)) ? 0 : 1;
	moveZ -= (mKeyboard->isKeyDown(OIS::KC_S) || mKeyboard->isKeyDown(OIS::KC_DOWN)) ? 0 : 1;
	float moveX = 0.0;
	moveX += (mKeyboard->isKeyDown(OIS::KC_A) || mKeyboard->isKeyDown(OIS::KC_LEFT)) ? 0 : 1;
	moveX -= (mKeyboard->isKeyDown(OIS::KC_D) || mKeyboard->isKeyDown(OIS::KC_RIGHT)) ? 0 : 1;

	Vector3 movement( moveX, 0.0, moveZ );
	mCameraNodeYaw->setPosition( mCameraNodeYaw->getPosition() +
			mCameraNodePitch->_getDerivedOrientation()*movement );

	return true; 
} 
// This gets called at the end of a frame.
bool SimpleFrameListener::frameEnded(const FrameEvent& evt) 
{ 
	return true; 
} 

void SimpleFrameListener::createScene( Ogre::Root* root )
{
	mRoot = root;

    //-----------------------------------------------------
    // 4 Create the SceneManager
    //
    //        ST_GENERIC = octree
    //        ST_EXTERIOR_CLOSE = simple terrain
    //        ST_EXTERIOR_FAR = nature terrain (depreciated)
    //        ST_EXTERIOR_REAL_FAR = paging landscape
    //        ST_INTERIOR = Quake3 BSP
    //----------------------------------------------------- 
    mSceneMgr = mRoot->createSceneManager(ST_GENERIC); 
 	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
 
    //----------------------------------------------------- 
    // 5 Create the camera 
    //----------------------------------------------------- 
    mCamera = mSceneMgr->createCamera("SimpleCamera"); 
	mCamera->setNearClipDistance( 1 );
	// The camera should be moved by moving mCameraNodeYaw. The Yaw node also controls cam Yaw, the 
	// pitch node is used to contol cam Pitch. Pitch is a child of Yaw, Camera is a child of Pitch
	mCameraNodeYaw = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mCameraNodePitch = mCameraNodeYaw->createChildSceneNode();
	mCameraNodePitch->attachObject( mCamera );
	mCameraNodeYaw->setPosition( 150.0, 100.0, 100.0 );
	mCameraNodeYaw->yaw( Degree( 30 ) );

	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 1500, 1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	entGround->setMaterialName("Grey");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( entGround );

	//----------------------------------------------------
	// Light up the Scene:
	//----------------------------------------------------
	Light* light = mSceneMgr->createLight();
	light->setType( Light::LT_POINT );
	light->setPosition( 0.0, 200, 0.0 );
	light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
}
