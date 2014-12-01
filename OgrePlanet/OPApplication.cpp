/*
Copyright (c) 2010 Anders Lingfors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "OPApplication.h"

#include "OPPlanet.h"
#include "OPNoiseppDataSource.h"
#include "OPIdentityDataSource.h"
#include "OPPatchMeshLoaderQueue.h"

#include <Threading/OgreDefaultWorkQueue.h>

namespace OgrePlanet
{
	void Application::go()
	{
		createRoot();
		defineResources();
		setupRenderSystem();
		createRenderWindow();
		initializeResourceGroups();
		setupScene();
		setupInputSystem();
		//testRTT();
		startRenderLoop();
		shutDown();
	}

	Application::~Application()
	{
		mPlanet.reset();

		mInputManager->destroyInputObject(mKeyboard);
		OIS::InputManager::destroyInputSystem(mInputManager);

		delete mRoot;
	}

	void Application::createRoot()
	{
#ifdef _DEBUG
		mRoot = new Ogre::Root("plugins_d.cfg");
#else
		mRoot = new Ogre::Root("plugins.cfg");
#endif
	}

	void Application::defineResources()
	{
		Ogre::String secName, typeName, archName;
		Ogre::ConfigFile cf;
#ifdef _DEBUG
    	cf.load("resources_d.cfg");
#else
    	cf.load("resources.cfg");
#endif
		
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;

			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
		}
	}

	void Application::setupRenderSystem()
	{
		if (!mRoot->showConfigDialog())
		{
			throw Ogre::Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
		}
	}

	void Application::createRenderWindow()
	{
		mWindow = mRoot->initialise(true, "OgrePlanet");
	}

	void Application::initializeResourceGroups()
	{
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	void Application::setupScene()
	{
		Ogre::DefaultWorkQueue * workQueue = (Ogre::DefaultWorkQueue *) mRoot->getWorkQueue();
		//workQueue->setWorkerThreadCount(4);
		//workQueue->startup(true);

		PatchMeshLoader::init(32);

		mPatchMeshLoaderQueue = new PatchMeshLoaderQueue();

		Ogre::SceneManager *mgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "Default SceneManager");
		mCam = mgr->createCamera("Camera");
		Ogre::Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(mCam);
		vp->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

		mFloatingOrigin = mgr->getRootSceneNode()->createChildSceneNode();

		mPlanetNode = mFloatingOrigin->createChildSceneNode();
		mPlanet = PlanetPtr(new Planet(mgr, mPlanetNode, 6371.0, 8.848, new NoiseppDataSource()));
		
		//Ogre::SceneNode *camNode = mgr->getRootSceneNode()->createChildSceneNode();
		Ogre::SceneNode *camNode = mPlanetNode->createChildSceneNode();
		camNode->attachObject(mCam);
		camNode->setPosition(Ogre::Vector3(0.0, 0.0, 10000.0));

		Ogre::Light * light = mgr->createLight("Light");
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		//light->setPosition(150000000 * Ogre::Vector3::UNIT_SCALE);
		//light->setAttenuation(1500000000, 1.0, 0.0, 0.0);
		//light->setDiffuseColour(Ogre::ColourValue::White);
		light->setDirection(-1.0, 0.0, -1.0);
		mgr->setAmbientLight(Ogre::ColourValue::White);
		
		//mCam->setPolygonMode(Ogre::PM_WIREFRAME);
		mCam->setNearClipDistance(0.001);
		mCam->setFarClipDistance(100000);

		Ogre::uint16 channel = workQueue->getChannel(PatchMeshLoaderQueue::getSingleton().getChannelName());
		workQueue->addRequestHandler(channel, mPatchMeshLoaderQueue);
		workQueue->addResponseHandler(channel, mPatchMeshLoaderQueue);
		workQueue->addRequest(channel, 0, Ogre::Any(0));
	}

	void Application::setupInputSystem()
	{
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		OIS::ParamList pl;
		Ogre::RenderWindow *win = mRoot->getAutoCreatedWindow();

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		mInputManager = OIS::InputManager::createInputSystem(pl);

		try
		{
			mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));
			mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, false));
			//mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, false));
		}
		catch (const OIS::Exception &e)
		{
			throw Ogre::Exception(42, e.eText, "Application::setupInputSystem");
		}
	}
	
	void Application::startRenderLoop()
	{
		mTrayManager = new OgreBites::SdkTrayManager("OgrePlanetTrayManager", mWindow, mMouse);
		mTrayManager->showFrameStats(OgreBites::TL_BOTTOMLEFT);
		mTrayManager->showLogo(OgreBites::TL_BOTTOMRIGHT);
		mTrayManager->hideCursor();

		mRoot->addFrameListener(this);

		Ogre::Timer timer;
		timer.reset();
		Ogre::Real timeFactor = 60.0 * 60.0 * 24.0;

		while (mPlanet->notifyPreRender() && mRoot->renderOneFrame() && mPlanet->notifyPostRender())
		{
			Ogre::Real time = timer.getMicroseconds() / 1000000.0;
			timer.reset();

			Ogre::Real groundLevel = 6371.0;
			Ogre::Real height = mCam->getParentSceneNode()->getPosition().length() - groundLevel;
			height *= 1000.0; // Convert km -> m
			height /= 0.3048; // Convert m -> feet
			
			Ogre::Real minSpeed = 463.0; // knot -> km/h
			Ogre::Real maxSpeed = 1000000 * minSpeed;
			
			Ogre::Real speed = minSpeed;
			
			if (height > 10000.0)
			{
				speed = minSpeed + 0.05 * (height - 10000.0);
			}

			speed = std::min(speed, maxSpeed);
				
			speed /= (60*60); // km/h -> km/s

			mKeyboard->capture();
			mMouse->capture();

			if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
			{
				break;
			}
			
			if (mKeyboard->isKeyDown(OIS::KC_SYSRQ))
			{
				mRoot->getAutoCreatedWindow()->writeContentsToTimestampedFile("OgrePlanet", ".bmp");
			}

			if (mKeyboard->isKeyDown(OIS::KC_F1))
			{
				if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
				{
					mCam->setPolygonMode(Ogre::PM_SOLID);
				}
				else
				{
					mCam->setPolygonMode(Ogre::PM_WIREFRAME);
				}
			}

			if (mKeyboard->isKeyDown(OIS::KC_F2))
			{
				if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
				{
					timeFactor = 60.0 * 60.0 * 24.0;
				}
				else
				{
					timeFactor = 60.0;
				}
			}

			mPlanetNode->yaw(Ogre::Radian((Ogre::Math::TWO_PI / timeFactor) * time));
			
			if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			{
				speed *= 100;
			}

			if (mKeyboard->isKeyDown(OIS::KC_Z))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::NEGATIVE_UNIT_Y, Ogre::Node::TS_LOCAL);
			}
			if (mKeyboard->isKeyDown(OIS::KC_X))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::UNIT_Y, Ogre::Node::TS_LOCAL);
			}
			if (mKeyboard->isKeyDown(OIS::KC_W))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Node::TS_LOCAL);
			}
			if (mKeyboard->isKeyDown(OIS::KC_S))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::UNIT_Z, Ogre::Node::TS_LOCAL);
			}
			if (mKeyboard->isKeyDown(OIS::KC_A))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Node::TS_LOCAL);
			}
			if (mKeyboard->isKeyDown(OIS::KC_D))
			{
				mCam->getParentSceneNode()->translate(speed * time * Ogre::Vector3::UNIT_X, Ogre::Node::TS_LOCAL);
			}
			
			if (mKeyboard->isKeyDown(OIS::KC_Q))
			{
				mCam->getParentSceneNode()->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian((time / 10.0) * Ogre::Math::TWO_PI));
			}
			if (mKeyboard->isKeyDown(OIS::KC_E))
			{
				mCam->getParentSceneNode()->rotate(Ogre::Vector3::UNIT_Z, -Ogre::Radian((time / 10.0) * Ogre::Math::TWO_PI));
			}
			mCam->getParentSceneNode()->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(-mMouse->getMouseState().Y.rel/100.0));
			mCam->getParentSceneNode()->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(-mMouse->getMouseState().X.rel/100.0));

			mFloatingOrigin->translate(-mCam->getParentSceneNode()->_getDerivedPosition());

			PatchMeshLoaderQueue::getSingleton().setCameraPosition(mCam->getParentSceneNode()->getPosition());

			if (!mKeyboard->isKeyDown(OIS::KC_LCONTROL))
			{
				mPlanet->setCameraPosition(mCam->getParentSceneNode()->getPosition());
			}
			
			Ogre::WindowEventUtilities::messagePump();
		}
	}

	void Application::shutDown()
	{
		PatchMeshLoader::cleanup();
		mPatchMeshLoaderQueue->setAbort();
	}

	bool Application::frameRenderingQueued(const Ogre::FrameEvent & evt)
	{
		return mTrayManager->frameRenderingQueued(evt);
	}
}