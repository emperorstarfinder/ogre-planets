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

#ifndef PATCH_H
#define PATCH_H

#include "OPDataSource.h"
#include "OPPatchMeshLoader.h"

#include <Ogre.h>
#include <boost/shared_array.hpp>
#include <boost/thread/mutex.hpp>

namespace OgrePlanet
{
	class Patch
	{
	public:
		Patch(
			const Ogre::String & name,
			const Ogre::String & materialName,
			Ogre::SceneManager * mgr,
			Ogre::SceneNode * parentNode,
			const Ogre::Vector3 & min,
			const Ogre::Vector3 & max,
			Ogre::Real texXMin,
			Ogre::Real texXMax,
			Ogre::Real texYMin,
			Ogre::Real texYMax,
			const Ogre::Real baseRadius,
			const Ogre::Real scalingFactor,
			DataSource * dataSource,
			int quads,
			Ogre::RenderQueueGroupID renderQueue,
			bool prepareInBackground = true,
			int depth = 0,
			int minDepth = 0,
			int maxDepth = -1,
			Patch * parent = 0,
			int position = 0);

		~Patch();

		void setCameraPosition(const Ogre::Vector3 & position);
		void setMaterialName(const Ogre::String & materialName);
		Ogre::String & getMaterialName();
		void setTextureSize(size_t size);
		bool isPrepared();
		bool isLoaded();
		bool isReady();
		bool isLeaf();
		std::list<Ogre::String> getNeighbourNameList();
		Ogre::String getLeftNeighbourName(Ogre::String patchName);
		Ogre::String getRightNeighbourName(Ogre::String patchName);
		Ogre::String getUpNeighbourName(Ogre::String patchName);
		Ogre::String getDownNeighbourName(Ogre::String patchName);
		Ogre::String rotateCW(Ogre::String patchName);
		Ogre::String rotateCCW(Ogre::String patchName);
		Ogre::String rotate180(Ogre::String patchName);
		void updateStitching();
		bool isVisible(Ogre::Camera * cam);
		void notifyPreRender();
		void notifyPostRender();
		bool geometryUpdated();

	private:
		static std::set<Ogre::String> patchNameSet;

		boost::shared_array<Ogre::Vector3> buildHeightMap();
		void show();
		void hide();
		bool destroyChildren();
		boost::shared_array<Ogre::Real> getHeightData();
		const Ogre::Vector3 getPlanetPositionWorldSpace();
		const Ogre::Quaternion Patch::getPlanetOrientationWorldSpace();

		Ogre::Entity * mEntity;
		Ogre::MeshPtr mMesh;
		Ogre::AxisAlignedBox mAABB;
		Patch * mSubPatch[4];
		DataSource * mDataSource;
		PatchMeshLoader * mPatchMeshLoader;

		Ogre::String mName;
		Ogre::String mMaterialName;
		Ogre::SceneManager * mMgr;
		Ogre::SceneNode * mNode;
		Ogre::Vector3 mMin;
		Ogre::Vector3 mMax;
		Ogre::Real mBaseRadius;
		Ogre::Real mScalingFactor;
		int mQuads;
		Ogre::RenderQueueGroupID mRenderQueue;
		Ogre::BackgroundProcessTicket mTicket;
		int mDepth;
		int mMinDepth;
		int mMaxDepth;
		Ogre::Real mTexXMin;
		Ogre::Real mTexXMax;
		Ogre::Real mTexYMin;
		Ogre::Real mTexYMax;
		Ogre::Vector3 mPatchCenter;

		Patch * mParent;
		boost::shared_array<Ogre::Real> mHeightData;

		Ogre::String leftNeighbour;
		Ogre::String rightNeighbour;
		Ogre::String upNeighbour;
		Ogre::String downNeighbour;

		Ogre::SceneNode * mParentSceneNode;

		bool mGeometryUpdated;
	};
}

#endif // PATCH_H