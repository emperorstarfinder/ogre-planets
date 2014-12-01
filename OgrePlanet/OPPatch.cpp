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

#include "OPPatch.h"

#include "OPPatchMeshLoaderQueue.h"
#include "OPUtil.h"
#include "OPStitching.h"

#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>

namespace OgrePlanet
{
	std::set<Ogre::String> Patch::patchNameSet;

	Patch::Patch(
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
		bool prepareInBackground,
		int depth,
		int minDepth,
		int maxDepth,
		Patch * parent,
		int position) :
	mName(name),
		mMaterialName(materialName),
		mMgr(mgr),
		mNode(0),
		mMin(min),
		mMax(max),
		mTexXMin(texXMin),
		mTexXMax(texXMax),
		mTexYMin(texYMin),
		mTexYMax(texYMax),
		mBaseRadius(baseRadius),
		mScalingFactor(scalingFactor),
		mDataSource(dataSource),
		mQuads(quads),
		mRenderQueue(renderQueue),
		mEntity(0),
		mTicket(0),
		mDepth(depth),
		mMinDepth(minDepth),
		mMaxDepth(maxDepth),
		mParent(parent),
		mHeightData(new Ogre::Real[(quads + 2*2 + 1) * (quads + 2*2 + 1)]),
		mParentSceneNode(parentNode),
		mGeometryUpdated(false)
	{
		//mPatchMeshLoader = new PatchMeshLoader(
		//	mDataSource,
		//	mQuads,
		//	mMin,
		//	mMax,
		//	texXMin,
		//	texXMax,
		//	texYMin,
		//	texYMax,
		//	mBaseRadius,
		//	mScalingFactor,
		//	mAABB,
		//	mHeightData,
		//	(parent != 0 ? mParent->getHeightData() : boost::shared_array<Ogre::Real>()),
		//	position);
		mPatchMeshLoader = new PatchMeshLoader(
			mDataSource,
			mQuads,
			mMin,
			mMax,
			mTexXMin,
			mTexXMax,
			mTexYMin,
			mTexYMax,
			mBaseRadius,
			mScalingFactor,
			mAABB,
			mHeightData,
			(parent != 0 ? mParent->getHeightData() : boost::shared_array<Ogre::Real>()),
			position);

		for (int i = 0; i < 4; i++)
		{
			mSubPatch[i] = 0;
		}

		mMesh = Ogre::MeshManager::getSingleton().createManual(mName + "Mesh",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			mPatchMeshLoader);

		if (prepareInBackground)
		{
			// Start generating the mesh in the background
			//mTicket = Ogre::ResourceBackgroundQueue::getSingleton().prepare(Ogre::MeshManager::getSingleton().getResourceType(),
			//	mName + "Mesh",
			//	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			//	true,
			//	mPatchMeshLoader);

			PatchMeshLoaderQueue::getSingleton().prepareMesh(mMesh, mPatchMeshLoader);
		}
		else
		{
			// Completely build the patch on this thread
			mMesh->prepare();

			if (mDepth == 0) {
				show();
			}
		}

		if (mParent) {
			Ogre::String ln = leftNeighbour = getLeftNeighbourName(mName);
			if (Ogre::StringUtil::match(mName, "*top*", false) && !Ogre::StringUtil::match(leftNeighbour, "*top*", false)) {
				 ln = leftNeighbour = rotateCCW(leftNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*bottom*", false) && !Ogre::StringUtil::match(leftNeighbour, "*bottom*", false)) {
				ln = leftNeighbour = rotateCW(leftNeighbour);
			}

			Ogre::String rn = rightNeighbour = getRightNeighbourName(mName);
			if (Ogre::StringUtil::match(mName, "*top*", false) && !Ogre::StringUtil::match(rightNeighbour, "*top*", false)) {
				rn = rightNeighbour = rotateCW(rightNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*bottom*", false) && !Ogre::StringUtil::match(rightNeighbour, "*bottom*", false)) {
				rn = rightNeighbour = rotateCCW(rightNeighbour);
			}

			Ogre::String un = upNeighbour = getUpNeighbourName(mName);
			if (Ogre::StringUtil::match(mName, "*top*", false) && !Ogre::StringUtil::match(upNeighbour, "*top*", false)) {
				un = upNeighbour = rotate180(upNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*left*", false) && !Ogre::StringUtil::match(upNeighbour, "*left*", false)) {
				un = upNeighbour = rotateCW(upNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*right*", false) && !Ogre::StringUtil::match(upNeighbour, "*right*", false)) {
				un = upNeighbour = rotateCCW(upNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*back*", false) && !Ogre::StringUtil::match(upNeighbour, "*back*", false)) {
				un = upNeighbour = rotate180(upNeighbour);
			}

			Ogre::String dn = downNeighbour = getDownNeighbourName(mName);
			if (Ogre::StringUtil::match(mName, "*bottom*", false) && !Ogre::StringUtil::match(downNeighbour, "*bottom*", false)) {
				dn = downNeighbour = rotate180(downNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*left*", false) && !Ogre::StringUtil::match(downNeighbour, "*left*", false)) {
				dn = downNeighbour = rotateCCW(downNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*right*", false) && !Ogre::StringUtil::match(downNeighbour, "*right*", false)) {
				dn = downNeighbour = rotateCW(downNeighbour);
			} else if (Ogre::StringUtil::match(mName, "*back*", false) && !Ogre::StringUtil::match(downNeighbour, "*back*", false)) {
				dn = downNeighbour = rotate180(downNeighbour);
			}
		}
	}

	// This destructor is not thread safe. When you delete a patch,
	// you better be damn sure that patch's mesh isn't waiting to be prepared
	// by the PatchMeshLoaderQueue.
	Patch::~Patch()
	{
		hide();
		patchNameSet.erase(mName);
		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
		delete mPatchMeshLoader;
	}

	void Patch::setCameraPosition(const Ogre::Vector3 & position)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mSubPatch[i])
			{
				mSubPatch[i]->setCameraPosition(position);
			}
		}

		if ((mEntity ||
			(mSubPatch[0] &&
			mSubPatch[1] &&
			mSubPatch[2] &&
			mSubPatch[3])) &&
			(mDepth < mMinDepth ||
			((mMaxDepth == -1 || mDepth < mMaxDepth) &&
			Util::distance(position - mPatchCenter, mAABB) < mAABB.getSize().length())))
		{
			if (mSubPatch[0] && mSubPatch[0]->isPrepared() &&
				mSubPatch[1] && mSubPatch[1]->isPrepared() &&
				mSubPatch[2] && mSubPatch[2]->isPrepared() &&
				mSubPatch[3] && mSubPatch[3]->isPrepared())
			{
				if (mParent &&
					(patchNameSet.find(leftNeighbour) == patchNameSet.end() ||
					patchNameSet.find(rightNeighbour) == patchNameSet.end() ||
					patchNameSet.find(upNeighbour) == patchNameSet.end() ||
					patchNameSet.find(downNeighbour) == patchNameSet.end()))
				{
					// Don't show children if it would cause a crack
					return;
				}

				// We are showing, but we are too close and our subPatches are ready to be shown
				// so show sub-patches and hide ourselves.
				for (int i = 0; i < 4; i++)
				{
					mSubPatch[i]->show();
				}

				hide();

				return;
			}
			else if (mSubPatch[0] == 0 ||
				mSubPatch[1] == 0 ||
				mSubPatch[2] == 0 ||
				mSubPatch[3] == 0)
			{
				Ogre::Vector3 center(
					mMin.x + (mMax.x - mMin.x)/2,
					mMin.y + (mMax.y - mMin.y)/2,
					mMin.z + (mMax.z - mMin.z)/2);

				Ogre::Vector3 topCenter;
				Ogre::Vector3 bottomCenter;
				Ogre::Vector3 leftCenter;
				Ogre::Vector3 rightCenter;

				if (mMin.x == mMax.x)
				{
					// This patch is perpendicular to the x axis
					// (right/left patches)
					topCenter = Ogre::Vector3(mMin.x, mMin.y, center.z);
					bottomCenter = Ogre::Vector3(mMax.x, mMax.y, center.z);
					leftCenter = Ogre::Vector3(mMin.x, center.y, mMin.z);
					rightCenter = Ogre::Vector3(mMax.x, center.y, mMax.z);
				}
				else if (mMin.y == mMax.y)
				{
					// This patch is perpendicular to the y axis
					// (top/bottom patches)
					topCenter = Ogre::Vector3(center.x, mMin.y, mMin.z);
					bottomCenter = Ogre::Vector3(center.x, mMax.y, mMax.z);
					leftCenter = Ogre::Vector3(mMin.x, mMin.y, center.z);
					rightCenter = Ogre::Vector3(mMax.x, mMax.y, center.z);
				}
				else if (mMin.z == mMax.z)
				{
					// This patch is perpendicular to the z axis
					// (front/back patches)
					topCenter = Ogre::Vector3(center.x, mMin.y, mMin.z);
					bottomCenter = Ogre::Vector3(center.x, mMax.y, mMax.z);
					leftCenter = Ogre::Vector3(mMin.x, center.y, mMin.z);
					rightCenter = Ogre::Vector3(mMax.x, center.y, mMax.z);
				}
				else
				{
					assert(false);
				}

				if (mSubPatch[0] == 0)
				{
					// "Upper left" patch
					mSubPatch[0] = new Patch(
						mName + "0",
						mMaterialName,
						mMgr,
						mParentSceneNode,
						mMin,
						center,
						(mDepth < mMaxDepth - 9) ? 0 : mTexXMin,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexXMin + (mTexXMax - mTexXMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? 0 : mTexYMin,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexYMin + (mTexYMax - mTexYMin) / 2.0,
						mBaseRadius,
						mScalingFactor,
						mDataSource,
						mQuads,
						mRenderQueue,
						true,
						mDepth+1,
						mMinDepth,
						mMaxDepth,
						this,
						0);
				}

				if (mSubPatch[1] == 0)
				{
					// "Upper right" patch
					mSubPatch[1] = new Patch(
						mName + "1",
						mMaterialName,
						mMgr,
						mParentSceneNode,
						topCenter,
						rightCenter,
						(mDepth < mMaxDepth - 9) ? 0 : mTexXMin + (mTexXMax - mTexXMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexXMax,
						(mDepth < mMaxDepth - 9) ? 0 : mTexYMin,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexYMin + (mTexYMax - mTexYMin) / 2.0,
						mBaseRadius,
						mScalingFactor,
						mDataSource,
						mQuads,
						mRenderQueue,
						true,
						mDepth+1,
						mMinDepth,
						mMaxDepth,
						this,
						1);
				}

				if (mSubPatch[2] == 0)
				{
					// "Lower left" patch
					mSubPatch[2] = new Patch(
						mName + "2",
						mMaterialName,
						mMgr,
						mParentSceneNode,
						leftCenter,
						bottomCenter,
						(mDepth < mMaxDepth - 9) ? 0 : mTexXMin,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexXMin + (mTexXMax - mTexXMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? 0 : mTexYMin + (mTexYMax - mTexYMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexYMax,
						mBaseRadius,
						mScalingFactor,
						mDataSource,
						mQuads,
						mRenderQueue,
						true,
						mDepth+1,
						mMinDepth,
						mMaxDepth,
						this,
						2);
				}

				if (mSubPatch[3] == 0)
				{
					// "Lower right" patch
					mSubPatch[3] = new Patch(
						mName + "3",
						mMaterialName,
						mMgr,
						mParentSceneNode,
						center,
						mMax,
						(mDepth < mMaxDepth - 9) ? 0 : mTexXMin + (mTexXMax - mTexXMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexXMax,
						(mDepth < mMaxDepth - 9) ? 0 : mTexYMin + (mTexYMax - mTexYMin) / 2.0,
						(mDepth < mMaxDepth - 9) ? Ogre::Math::Pow(2.0, mMaxDepth - mDepth - 1.0) : mTexYMax,
						mBaseRadius,
						mScalingFactor,
						mDataSource,
						mQuads,
						mRenderQueue,
						true,
						mDepth+1,
						mMinDepth,
						mMaxDepth,
						this,
						3);
				}
			}
		}
		else
		{
			// We are too far away to split.
			// If we have children, we have to remove them.
			// This can only be done after the children's meshes have been prepared (this is done on another thread).
			if (isLoaded())
			{
				// We have no entity, but nothing is preventing us from creating one.
				// (I.e. the mesh we need is either prepared or loaded.)

				if ((!mSubPatch[0] || mSubPatch[0]->isLeaf()) &&
					(!mSubPatch[1] || mSubPatch[1]->isLeaf()) &&
					(!mSubPatch[2] || mSubPatch[2]->isLeaf()) &&
					(!mSubPatch[3] || mSubPatch[3]->isLeaf()))
				{
					// If we have no children, or if the children we have are
					// leafs, it's time to hide the children and show ourselves instead.
					if (!mEntity)
					{
						show();
					}

					for (int i = 0; i < 4; i++)
					{
						if (mSubPatch[i] && mSubPatch[i]->isReady())
						{
							mSubPatch[i]->hide();
							delete mSubPatch[i];
							mSubPatch[i] = 0;
						}
					}
				}
			}
		}
	}

	bool Patch::isPrepared()
	{
		return mMesh->isPrepared();
	}

	bool Patch::isLoaded()
	{
		return mMesh->isLoaded();
	}

	bool Patch::isReady()
	{
		return mMesh->isPrepared() || mMesh->isLoaded();
	}

	bool Patch::isLeaf()
	{
		return !(mSubPatch[0] ||
			mSubPatch[1] ||
			mSubPatch[2] ||
			mSubPatch[3]);
	}

	void Patch::show()
	{
		if (!mEntity)
		{
			mEntity = mMgr->createEntity(mName + "Entity", mName + "Mesh");
			mEntity->setMaterialName(mMaterialName);
			mEntity->setRenderQueueGroupAndPriority(mRenderQueue, mMaxDepth - mDepth);

			for (unsigned int i = 0; i < mEntity->getNumSubEntities(); i++)
			{
				Ogre::Real time = (Ogre::Real)Ogre::Root::getSingleton().getTimer()->getMilliseconds();
				mEntity->getSubEntity(i)->setCustomParameter(3, Ogre::Vector4(time, 0.0, 0.0, 0.0));
			}

			//for (unsigned int i = 0; i < mEntity->getNumSubEntities(); i++)
			//{
			//	Ogre::GpuProgramPtr fragProg = mEntity->getSubEntity(i)->
			//		getMaterial()->
			//		getBestTechnique()->
			//		getPass(0)->
			//		getFragmentProgram();
			//	
			//	fragProg->setParameter("baseRadius", boost::lexical_cast<Ogre::String, Ogre::Real>(mBaseRadius));
			//	fragProg->setParameter("scalingFactor", boost::lexical_cast<Ogre::String, Ogre::Real>(mScalingFactor));
			//}
		}

		mPatchCenter = mPatchMeshLoader->getCenter();

		//if (mParent != 0)
		//{
		//	mNode = mParentNode->createChildSceneNode(mPatchCenter - mParent->mPatchCenter);
		//}
		//else
		//{
		//	mNode = mParentNode->createChildSceneNode(mPatchCenter);
		//}
		if (!mNode)
		{
			mNode = mParentSceneNode->createChildSceneNode(mPatchCenter);
		}
		
		if (!mEntity->isAttached())
		{
			mNode->attachObject(mEntity);
		}
		//mPatchNormal = mAABB.getCenter().normalisedCopy();
		//mPatchCenter = mBaseRadius * mPatchNormal;
		patchNameSet.insert(mName);
		mGeometryUpdated = true;
	}

	void Patch::hide()
	{
		if (mEntity != 0)
		{
			mNode->detachObject(mEntity);
			mMgr->destroyEntity(mEntity);
			mEntity = 0;

			mMgr->destroySceneNode(mNode);
			mNode = 0;
		}

		mGeometryUpdated = true;
	}

	std::list<Ogre::String> Patch::getNeighbourNameList()
	{
		std::list<Ogre::String> neighbourNameList;

		neighbourNameList.push_back(getLeftNeighbourName(mName));
		neighbourNameList.push_back(getRightNeighbourName(mName));
		neighbourNameList.push_back(getUpNeighbourName(mName));
		neighbourNameList.push_back(getDownNeighbourName(mName));

		return neighbourNameList;
	}

	Ogre::String Patch::getLeftNeighbourName(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "front")) {
			return patchName.substr(0, patchName.length()-5) + "Left";
		} else if (Ogre::StringUtil::endsWith(patchName, "back")) {
			return patchName.substr(0, patchName.length()-4) + "Right";
		} else if (Ogre::StringUtil::endsWith(patchName, "left")) {
			return patchName.substr(0, patchName.length()-4) + "Back";
		} else if (Ogre::StringUtil::endsWith(patchName, "right")) {
			return patchName.substr(0, patchName.length()-5) + "Front";
		} else if (Ogre::StringUtil::endsWith(patchName, "top")) {
			return patchName.substr(0, patchName.length()-3) + "Left";
		} else if (Ogre::StringUtil::endsWith(patchName, "bottom")) {
			return patchName.substr(0, patchName.length()-6) + "Left";
		}

		size_t nameLength = patchName.length();
		Ogre::String parentName = patchName.substr(0, nameLength-1);

		Ogre::String idString = patchName.substr(nameLength-1, 1);
		int id = boost::lexical_cast<int, Ogre::String>(idString);

		switch (id)
		{
		case 0:
			return getLeftNeighbourName(parentName) + "1";
		case 1:
			return parentName + "0";
		case 2:
			return getLeftNeighbourName(parentName) + "3";
		case 3:
			return parentName + "2";
		}
	}

	Ogre::String Patch::getRightNeighbourName(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "front")) {
			return patchName.substr(0, patchName.length()-5) + "Right";
		} else if (Ogre::StringUtil::endsWith(patchName, "back")) {
			return patchName.substr(0, patchName.length()-4) + "Left";
		} else if (Ogre::StringUtil::endsWith(patchName, "left")) {
			return patchName.substr(0, patchName.length()-4) + "Front";
		} else if (Ogre::StringUtil::endsWith(patchName, "right")) {
			return patchName.substr(0, patchName.length()-5) + "Back";
		} else if (Ogre::StringUtil::endsWith(patchName, "top")) {
			return patchName.substr(0, patchName.length()-3) + "Right";
		} else if (Ogre::StringUtil::endsWith(patchName, "bottom")) {
			return patchName.substr(0, patchName.length()-6) + "Right";
		}

		size_t nameLength = patchName.length();
		Ogre::String parentName = patchName.substr(0, nameLength-1);

		Ogre::String idString = patchName.substr(nameLength-1, 1);
		int id = boost::lexical_cast<int, Ogre::String>(idString);

		switch (id)
		{
		case 0:
			return parentName + "1";
		case 1:
			return getRightNeighbourName(parentName) + "0";
		case 2:
			return parentName + "3";
		case 3:
			return getRightNeighbourName(parentName) + "2";
		}
	}

	Ogre::String Patch::getUpNeighbourName(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "front")) {
			return patchName.substr(0, patchName.length()-5) + "Top";
		} else if (Ogre::StringUtil::endsWith(patchName, "back")) {
			return patchName.substr(0, patchName.length()-4) + "Top";
		} else if (Ogre::StringUtil::endsWith(patchName, "left")) {
			return patchName.substr(0, patchName.length()-4) + "Top";
		} else if (Ogre::StringUtil::endsWith(patchName, "right")) {
			return patchName.substr(0, patchName.length()-5) + "Top";
		} else if (Ogre::StringUtil::endsWith(patchName, "top")) {
			return patchName.substr(0, patchName.length()-3) + "Back";
		} else if (Ogre::StringUtil::endsWith(patchName, "bottom")) {
			return patchName.substr(0, patchName.length()-6) + "Front";
		}

		size_t nameLength = patchName.length();
		Ogre::String parentName = patchName.substr(0, nameLength-1);

		Ogre::String idString = patchName.substr(nameLength-1, 1);
		int id = boost::lexical_cast<int, Ogre::String>(idString);

		switch (id)
		{
		case 0:
			return getUpNeighbourName(parentName) + "2";
		case 1:
			return getUpNeighbourName(parentName) + "3";
		case 2:
			return parentName + "0";
		case 3:
			return parentName + "1";
		}
	}

	Ogre::String Patch::getDownNeighbourName(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "front")) {
			return patchName.substr(0, patchName.length()-5) + "Bottom";
		} else if (Ogre::StringUtil::endsWith(patchName, "back")) {
			return patchName.substr(0, patchName.length()-4) + "Bottom";
		} else if (Ogre::StringUtil::endsWith(patchName, "left")) {
			return patchName.substr(0, patchName.length()-4) + "Bottom";
		} else if (Ogre::StringUtil::endsWith(patchName, "right")) {
			return patchName.substr(0, patchName.length()-5) + "Bottom";
		} else if (Ogre::StringUtil::endsWith(patchName, "top")) {
			return patchName.substr(0, patchName.length()-3) + "Front";
		} else if (Ogre::StringUtil::endsWith(patchName, "bottom")) {
			return patchName.substr(0, patchName.length()-6) + "Back";
		}

		size_t nameLength = patchName.length();
		Ogre::String parentName = patchName.substr(0, nameLength-1);

		if (parentName == "")
		{
			return "";
		}

		Ogre::String idString = patchName.substr(nameLength-1, 1);
		int id = boost::lexical_cast<int, Ogre::String>(idString);

		switch (id)
		{
		case 0:
			return parentName + "2";
		case 1:
			return parentName + "3";
		case 2:
			return getDownNeighbourName(parentName) + "0";
		case 3:
			return getDownNeighbourName(parentName) + "1";
		}
	}

	void Patch::setMaterialName(const Ogre::String & materialName)
	{
		mMaterialName = materialName;

		if (mEntity)
		{
			for (unsigned int i = 0; i < mEntity->getNumSubEntities(); i++)
			{
				mEntity->getSubEntity(i)->setMaterialName(materialName);
				mEntity->getSubEntity(i)->setMaterialName(materialName + "DepthPass");
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (mSubPatch[i])
			{
				mSubPatch[i]->setMaterialName(materialName);
			}
		}
	}

	Ogre::String & Patch::getMaterialName()
	{
		return mMaterialName;
	}

	void Patch::setTextureSize(size_t size)
	{
		if (mEntity)
		{
			for (unsigned int i = 0; i < mEntity->getNumSubEntities(); i++)
			{
				mEntity->getSubEntity(i)->setCustomParameter(1, Ogre::Vector4((float)size, 0.0, 0.0, 0.0));
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (mSubPatch[i])
			{
				mSubPatch[i]->setTextureSize(size);
			}
		}
	}

	boost::shared_array<Ogre::Real> Patch::getHeightData()
	{
		return mHeightData;
	}

	Ogre::String Patch::rotateCW(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "0")) {
			return rotateCW(patchName.substr(0, patchName.length()-1)) + "1";
		} else if (Ogre::StringUtil::endsWith(patchName, "1")) {
			return rotateCW(patchName.substr(0, patchName.length()-1)) + "3";
		} else if (Ogre::StringUtil::endsWith(patchName, "2")) {
			return rotateCW(patchName.substr(0, patchName.length()-1)) + "0";
		} else if (Ogre::StringUtil::endsWith(patchName, "3")) {
			return rotateCW(patchName.substr(0, patchName.length()-1)) + "2";
		}

		return patchName;
	}

	Ogre::String Patch::rotateCCW(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "0")) {
			return rotateCCW(patchName.substr(0, patchName.length()-1)) + "2";
		} else if (Ogre::StringUtil::endsWith(patchName, "1")) {
			return rotateCCW(patchName.substr(0, patchName.length()-1)) + "0";
		} else if (Ogre::StringUtil::endsWith(patchName, "2")) {
			return rotateCCW(patchName.substr(0, patchName.length()-1)) + "3";
		} else if (Ogre::StringUtil::endsWith(patchName, "3")) {
			return rotateCCW(patchName.substr(0, patchName.length()-1)) + "1";
		}

		return patchName;
	}

	Ogre::String Patch::rotate180(Ogre::String patchName)
	{
		if (Ogre::StringUtil::endsWith(patchName, "0")) {
			return rotate180(patchName.substr(0, patchName.length()-1)) + "3";
		} else if (Ogre::StringUtil::endsWith(patchName, "1")) {
			return rotate180(patchName.substr(0, patchName.length()-1)) + "2";
		} else if (Ogre::StringUtil::endsWith(patchName, "2")) {
			return rotate180(patchName.substr(0, patchName.length()-1)) + "1";
		} else if (Ogre::StringUtil::endsWith(patchName, "3")) {
			return rotate180(patchName.substr(0, patchName.length()-1)) + "0";
		}

		return patchName;
	}

	void Patch::updateStitching()
	{
		if (mEntity)
		{
			int index = 0;
			Ogre::Vector4 stitch = Ogre::Vector4::ZERO;

			if (mParent) {
				if (patchNameSet.find(leftNeighbour) == patchNameSet.end()) {
					index |= STITCHING_W;
					stitch.x = 1.0;
				}
				if (patchNameSet.find(rightNeighbour) == patchNameSet.end()) {
					index |= STITCHING_E;
					stitch.y = 1.0;
				}
				if (patchNameSet.find(upNeighbour) == patchNameSet.end()) {
					index |= STITCHING_N;
					stitch.z = 1.0;
				}
				if (patchNameSet.find(downNeighbour) == patchNameSet.end()) {
					index |= STITCHING_S;
					stitch.w = 1.0;
				}
			}

			mMesh->getSubMesh(0)->indexData->indexCount = PatchMeshLoader::indexBuffer[index]->getNumIndexes();
			mMesh->getSubMesh(0)->indexData->indexBuffer = PatchMeshLoader::indexBuffer[index];

			for (unsigned int i = 0; i < mEntity->getNumSubEntities(); i++)
			{
				Ogre::Real time = (Ogre::Real)Ogre::Root::getSingleton().getTimer()->getMilliseconds();

				mEntity->getSubEntity(i)->setCustomParameter(4, Ogre::Vector4(time, 0.0, 0.0, 0.0));
				mEntity->getSubEntity(i)->setCustomParameter(5, stitch);
			}
		}

		for (int i = 0; i < 4; i++) {
			if (mSubPatch[i]) {
				mSubPatch[i]->updateStitching();
			}
		}
	}

	bool Patch::isVisible(Ogre::Camera * cam) {
		for (int i = 0; i < 4; i++) {
			if (mSubPatch[i] && mSubPatch[i]->isVisible(cam)) {
				return true;
			}
		}

		if (mEntity && mEntity->isAttached() && cam->isVisible(mEntity->getBoundingBox())) {
			return true;
		}

		return false;
	}

	//void Patch::notifyPreRender(Ogre::Camera * cam) {
	void Patch::notifyPreRender() {
		for (int i = 0; i < 4; i++) {
			if (mSubPatch[i]) {
				mSubPatch[i]->notifyPreRender();
			}
		}

		Ogre::Real baseRadius = 6371.0; // Hack, because otherwise atmosphere isn't culled correctly
											// (It has another baseRadius...)

		// Do horizon culling
		Ogre::Vector3 planetPosition = getPlanetPositionWorldSpace();
		Ogre::Vector3 planetDirection = planetPosition.normalisedCopy();
		Ogre::Vector3 patchPosition = planetPosition + (getPlanetOrientationWorldSpace() * mPatchCenter);
		Ogre::Vector3 patchDirection = patchPosition.normalisedCopy();
		Ogre::Real patchRadius = mAABB.getHalfSize().length();

		Ogre::Real cosAngleToPlanetHorizon = Ogre::Math::Sqrt(planetPosition.squaredLength() - baseRadius*baseRadius) / planetPosition.length();
		Ogre::Radian angleToPlanetHorizon = Ogre::Math::ACos(cosAngleToPlanetHorizon);

		Ogre::Real cosAngleToPatchHorizon = Ogre::Math::Sqrt(patchPosition.squaredLength() - patchRadius*patchRadius) / patchPosition.length();
		Ogre::Radian angleToPatchHorizon = Ogre::Math::ACos(cosAngleToPatchHorizon);

		Ogre::Real distanceToPlanetHorizon = Ogre::Math::Sqrt(planetPosition.squaredLength() - baseRadius*baseRadius);
		Ogre::Radian angleBetweenPlanetAndPatch = Ogre::Math::ACos(planetDirection.dotProduct(patchDirection));

		bool isBehindHorizonPlane = ((patchPosition.dotProduct(planetDirection) - patchRadius) > (distanceToPlanetHorizon * cosAngleToPlanetHorizon));
		//bool isBehindHorizonPlane = true;
		bool isInHorizonCone = ((angleBetweenPlanetAndPatch + angleToPatchHorizon) < angleToPlanetHorizon);
		//bool isInHorizonCone = true;

		if (mNode && mEntity && mEntity->isAttached() && isBehindHorizonPlane && isInHorizonCone)
		{
			mNode->detachObject(mEntity);
		}
		else if (mNode && mEntity && !mEntity->isAttached() && !(isBehindHorizonPlane && isInHorizonCone))
		{
			mNode->attachObject(mEntity);
		}

		if (mEntity && mEntity->isAttached())
		{
			for (int i = 0; i < mEntity->getNumSubEntities(); i++)
			{
				mEntity->getSubEntity(i)->setCustomParameter(6, Ogre::Vector4(mPatchCenter.x, mPatchCenter.y, mPatchCenter.z, 0.0));
			}
		}
	}

	void Patch::notifyPostRender() {
		for (int i = 0; i < 4; i++) {
			if (mSubPatch[i]) {
				mSubPatch[i]->notifyPostRender();
			}
		}

		mGeometryUpdated = false;
	}

	const Ogre::Vector3 Patch::getPlanetPositionWorldSpace()
	{
		//if (mParent)
		//{
		//	return mParent->getPlanetPositionWorldSpace();
		//}
		//else
		//{
		//	return mParentNode->_getDerivedPosition();
		//}

		return mParentSceneNode->_getDerivedPosition();
	}

	const Ogre::Quaternion Patch::getPlanetOrientationWorldSpace()
	{
		//if (mParent)
		//{
		//	return mParent->getPlanetOrientationWorldSpace();
		//}
		//else
		//{
		//	return mParentNode->_getDerivedOrientation();
		//}

		return mParentSceneNode->_getDerivedOrientation();
	}

	bool Patch::geometryUpdated()
	{
		return ((mSubPatch[0] && mSubPatch[0]->geometryUpdated()) ||
			(mSubPatch[1] && mSubPatch[1]->geometryUpdated()) ||
			(mSubPatch[2] && mSubPatch[2]->geometryUpdated()) ||
			(mSubPatch[3] && mSubPatch[3]->geometryUpdated()) ||
			mGeometryUpdated);
	}
}