////////////////////////////////////////////////////////////////////////// 
// class SimpleFrameListener 
////////////////////////////////////////////////////////////////////////// 

#ifndef SIMPLEFRAMELISTENER_H
#define SIMPLEFRAMELISTENER_H

#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>

using namespace Ogre; 
class SimpleFrameListener : public FrameListener 
{ 
public: 
    SimpleFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse);
    // This gets called before the next frame is beeing rendered.
    bool frameStarted(const FrameEvent& evt);
    // This gets called at the end of a frame.
    bool frameEnded(const FrameEvent& evt);

	Camera* getCamera(){ return mCamera; }

	void createScene( Ogre::Root* root );
private: 
    OIS::Keyboard* mKeyboard; 
    OIS::Mouse* mMouse; 

	Root* mRoot;

	SceneManager* mSceneMgr;
	Camera* mCamera;

	SceneNode* mCameraNodeYaw;
	SceneNode* mCameraNodePitch;
}; 

#endif
