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

#ifndef PLANET_H
#define PLANET_H

#include "OPPatch.h"

#include <boost/shared_ptr.hpp>

namespace OgrePlanet
{
	class Planet;
	typedef boost::shared_ptr<Planet> PlanetPtr;

	class Planet
	{
	public:
		Planet(
			Ogre::SceneManager * mgr,
			Ogre::SceneNode * sceneNode,
			const Ogre::Real baseRadius,
			const Ogre::Real scalingFactor,
			DataSource * dataSource);
		~Planet();

		void setCameraPosition(const Ogre::Vector3 & position);
		void dumpPlanetTextures();
		bool notifyPreRender();
		bool notifyPostRender();
	protected:
	private:
		bool allTexturesPrepared();

		Patch * mSurfaceSide[6];
		Ogre::TexturePtr mCurrentSurfaceSideTexture[6];
		Ogre::TexturePtr mNextSurfaceSideTexture[6];
		Ogre::MaterialPtr mSurfaceMaterial[6];
		Patch * mOceanSide[6];
		Patch * mSkySide[6];
		Ogre::SceneNode * mSceneNode;
		Ogre::SceneManager * mMgr;
		Ogre::Real mBaseRadius;
		Ogre::Real mScalingFactor;
		Ogre::Real mOldNearClipDistance;
		Ogre::Real mOldFarClipDistance;
		int mResolution;
		DataSource * mDataSource;
		DataSource * mIdentityDataSource;
		Ogre::Vector3 mRightMin;
		Ogre::Vector3 mRightMax;
		Ogre::Vector3 mLeftMin;
		Ogre::Vector3 mLeftMax;
		Ogre::Vector3 mTopMin;
		Ogre::Vector3 mTopMax;
		Ogre::Vector3 mBottomMin;
		Ogre::Vector3 mBottomMax;
		Ogre::Vector3 mFrontMin;
		Ogre::Vector3 mFrontMax;
		Ogre::Vector3 mBackMin;
		Ogre::Vector3 mBackMax;
		Ogre::Real mNear;
		Ogre::Real mFar;
		int mRepetition;
		Ogre::StaticGeometry * mStaticGeometry;
		Ogre::SceneNode * mStaticGeometrySceneNode;
	};
}

#endif // PLANET_H