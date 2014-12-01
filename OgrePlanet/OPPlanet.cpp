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

#include "OPPlanet.h"
#include "OPIdentityDataSource.h"
#include "OPUtil.h"

#include "boost/lexical_cast.hpp"

namespace OgrePlanet
{
	Planet::Planet(
		Ogre::SceneManager * mgr,
		Ogre::SceneNode * sceneNode,
		const Ogre::Real baseRadius,
		const Ogre::Real scalingFactor,
		DataSource * dataSource) :
	mMgr(mgr),
		mSceneNode(sceneNode),
		mBaseRadius(baseRadius),
		mScalingFactor(scalingFactor),
		mDataSource(dataSource),
		mIdentityDataSource(new IdentityDataSource()),
		mResolution(1),
		mRepetition(0),
		mStaticGeometry(0)
	{
		Ogre::MaterialPtr baseMaterial = Ogre::MaterialManager::getSingleton().getByName("OgrePlanet/TerrainPhong");
		//Ogre::MaterialPtr baseMaterial = Ogre::MaterialManager::getSingleton().getByName("OgrePlanet/Sun");

		mRightMin = Ogre::Vector3(1.0, 1.0, 1.0);
		mRightMax = Ogre::Vector3(1.0, -1.0, -1.0);
		//mSurfaceMaterial[0] = baseMaterial->clone(baseMaterial->getName() + "Right");
		mSurfaceMaterial[0] = baseMaterial;
		mSurfaceSide[0] = new Patch(
			"SurfaceRight",
			mSurfaceMaterial[0]->getName(),
			mgr,
			sceneNode,
			mRightMin,
			mRightMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		mLeftMin = Ogre::Vector3(-1.0, 1.0, -1.0);
		mLeftMax = Ogre::Vector3(-1.0, -1.0, 1.0);
		//mSurfaceMaterial[1] = baseMaterial->clone(baseMaterial->getName() + "Left");
		mSurfaceMaterial[1] = baseMaterial;
		mSurfaceSide[1] = new Patch(
			"SurfaceLeft",
			mSurfaceMaterial[1]->getName(),
			mgr,
			sceneNode,
			mLeftMin,
			mLeftMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		mTopMin = Ogre::Vector3(-1.0, 1.0, -1.0);
		mTopMax = Ogre::Vector3(1.0, 1.0, 1.0);
		//mSurfaceMaterial[2] = baseMaterial->clone(baseMaterial->getName() + "Top");
		mSurfaceMaterial[2] = baseMaterial;
		mSurfaceSide[2] = new Patch(
			"SurfaceTop",
			mSurfaceMaterial[2]->getName(),
			mgr,
			sceneNode,
			mTopMin,
			mTopMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		mBottomMin = Ogre::Vector3(-1.0, -1.0, 1.0);
		mBottomMax = Ogre::Vector3(1.0, -1.0, -1.0);
		//mSurfaceMaterial[3] = baseMaterial->clone(baseMaterial->getName() + "Bottom");
		mSurfaceMaterial[3] = baseMaterial;
		mSurfaceSide[3] = new Patch(
			"SurfaceBottom",
			mSurfaceMaterial[3]->getName(),
			mgr,
			sceneNode,
			mBottomMin,
			mBottomMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		mFrontMin = Ogre::Vector3(-1.0, 1.0, 1.0);
		mFrontMax = Ogre::Vector3(1.0, -1.0, 1.0);
		//mSurfaceMaterial[4] = baseMaterial->clone(baseMaterial->getName() + "Front");
		mSurfaceMaterial[4] = baseMaterial;
		mSurfaceSide[4] = new Patch(
			"SurfaceFront",
			mSurfaceMaterial[4]->getName(),
			mgr,
			sceneNode,
			mFrontMin,
			mFrontMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		mBackMin = Ogre::Vector3(1.0, 1.0, -1.0);
		mBackMax = Ogre::Vector3(-1.0, -1.0, -1.0);
		//mSurfaceMaterial[5] = baseMaterial->clone(baseMaterial->getName() + "Back");
		mSurfaceMaterial[5] = baseMaterial;
		mSurfaceSide[5] = new Patch(
			"SurfaceBack",
			mSurfaceMaterial[5]->getName(),
			mgr,
			sceneNode,
			mBackMin,
			mBackMax,
			0,
			Ogre::Math::Pow(2.0, 20),
			0,
			Ogre::Math::Pow(2.0, 20),
			baseRadius,
			scalingFactor,
			dataSource,
			32,
			Ogre::RENDER_QUEUE_MAIN,
			false,
			0,
			4,
			15);

		Ogre::String oceanMaterialName = "OgrePlanet/Ocean";
		//Ogre::String oceanMaterialName = "OgrePlanet/TerrainGouraud";

		mOceanSide[0] = new Patch(
			"OceanRight",
			oceanMaterialName,
			mgr,
			sceneNode,
			mRightMin,
			mRightMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		mOceanSide[1] = new Patch(
			"OceanLeft",
			oceanMaterialName,
			mgr,
			sceneNode,
			mLeftMin,
			mLeftMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		mOceanSide[2] = new Patch(
			"OceanTop",
			oceanMaterialName,
			mgr,
			sceneNode,
			mTopMin,
			mTopMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		mOceanSide[3] = new Patch(
			"OceanBottom",
			oceanMaterialName,
			mgr,
			sceneNode,
			mBottomMin,
			mBottomMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		mOceanSide[4] = new Patch(
			"OceanFront",
			oceanMaterialName,
			mgr,
			sceneNode,
			mFrontMin,
			mFrontMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		mOceanSide[5] = new Patch(
			"OceanBack",
			oceanMaterialName,
			mgr,
			sceneNode,
			mBackMin,
			mBackMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_4,
			false,
			0,
			4,
			15);

		Ogre::String skyMaterialName = "OgrePlanet/Sky";
		//Ogre::String skyMaterialName = "OgrePlanet/TerrainGouraud";

		mSkySide[0] = new Patch(
			"SkyRight",
			skyMaterialName,
			mgr,
			sceneNode,
			mRightMin,
			mRightMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);

		mSkySide[1] = new Patch(
			"SkyLeft",
			skyMaterialName,
			mgr,
			sceneNode,
			mLeftMin,
			mLeftMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);

		mSkySide[2] = new Patch(
			"SkyTop",
			skyMaterialName,
			mgr,
			sceneNode,
			mTopMin,
			mTopMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);

		mSkySide[3] = new Patch(
			"SkyBottom",
			skyMaterialName,
			mgr,
			sceneNode,
			mBottomMin,
			mBottomMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);

		mSkySide[4] = new Patch(
			"SkyFront",
			skyMaterialName,
			mgr,
			sceneNode,
			mFrontMin,
			mFrontMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);

		mSkySide[5] = new Patch(
			"SkyBack",
			skyMaterialName,
			mgr,
			sceneNode,
			mBackMin,
			mBackMax,
			0.0,
			1.0,
			0.0,
			1.0,
			baseRadius + 20,
			0.0,
			mIdentityDataSource,
			32,
			Ogre::RENDER_QUEUE_6,
			false,
			0,
			0,
			7);
	}

	Planet::~Planet()
	{
		// mMgr->removeRenderQueueListener(this);

		for (int i = 0; i < 6; i++)
		{
			delete mSurfaceSide[i];
			delete mOceanSide[i];
			delete mSkySide[i];
		}
	}

	void Planet::setCameraPosition(const Ogre::Vector3 & position)
	{
		for (int i = 0; i < 6; i++)
		{
			mSurfaceSide[i]->setCameraPosition(position);
			mOceanSide[i]->setCameraPosition(position);
			mSkySide[i]->setCameraPosition(position);
		}

		for (int i = 0; i < 6; i++) {
			mSurfaceSide[i]->updateStitching();
			mOceanSide[i]->updateStitching();
			mSkySide[i]->updateStitching();
		}
	}

	bool Planet::notifyPreRender()
	{
		//if (!mStaticGeometry)
		//{
		//	mStaticGeometry = mMgr->createStaticGeometry("PlanetStaticGeometry");
		//	mStaticGeometrySceneNode = mSceneNode->getParentSceneNode()->createChildSceneNode();
		//}

		//bool geometryUpdated = false;

		for (int i = 0; i < 6; i++)
		{
			mSurfaceSide[i]->notifyPreRender();
			mOceanSide[i]->notifyPreRender();
			mSkySide[i]->notifyPreRender();

			//if (mSurfaceSide[i]->geometryUpdated() ||
			//	mOceanSide[i]->geometryUpdated() ||
			//	mSkySide[i]->geometryUpdated())
			//{
			//	geometryUpdated = true;
			//}
		}

		//if (geometryUpdated)
		//{
		//	mStaticGeometry->reset();

		//	mStaticGeometry->addSceneNode(mSceneNode);
		//	mStaticGeometry->build();
		//}

		//mStaticGeometrySceneNode->setPosition(mSceneNode->getPosition());
		//mStaticGeometrySceneNode->setOrientation(mSceneNode->getOrientation());
		//mSceneNode->getParentSceneNode()->removeChild(mSceneNode);

		return true;
	}

	bool Planet::notifyPostRender()
	{
		//for (int i = 0; i < 6; i++)
		//{
		//	mSurfaceSide[i]->notifyPostRender();
		//	mOceanSide[i]->notifyPostRender();
		//	mSkySide[i]->notifyPostRender();
		//}

		//mStaticGeometrySceneNode->getParentSceneNode()->addChild(mSceneNode);

		return true;
	}
}