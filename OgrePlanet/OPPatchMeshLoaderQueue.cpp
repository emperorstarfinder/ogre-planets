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

#include "OPPatchMeshLoaderQueue.h"

template<> OgrePlanet::PatchMeshLoaderQueue* Ogre::Singleton<OgrePlanet::PatchMeshLoaderQueue>::ms_Singleton = 0;

namespace OgrePlanet
{
	PatchMeshLoaderQueue* PatchMeshLoaderQueue::getSingletonPtr(void)
    {
        return ms_Singleton;
    }

	PatchMeshLoaderQueue& PatchMeshLoaderQueue::getSingleton(void)
    {
        assert( ms_Singleton );  return ( *ms_Singleton );
    }

	PatchMeshLoaderQueue::PatchMeshLoaderQueue() :
		mMaxDepth(-1),
		mAbort(false)
	{}

	void PatchMeshLoaderQueue::setCameraPosition(const Ogre::Vector3 & pos)
	{
		OGRE_LOCK_MUTEX(cameraPosMutex)
		mCameraPos = pos;
	}

	void PatchMeshLoaderQueue::prepareMesh(Ogre::MeshPtr mesh, PatchMeshLoader * patchMeshLoader)
	{
		Ogre::Real baseRadius = patchMeshLoader->getBaseRadius();
		Ogre::Vector3 min = baseRadius * patchMeshLoader->getMin().normalisedCopy();
		Ogre::Vector3 max = baseRadius * patchMeshLoader->getMax().normalisedCopy();
		AxisAlignedBoxPtr aabbPtr = AxisAlignedBoxPtr(new Ogre::AxisAlignedBox(
			std::min(min.x, max.x),
			std::min(min.y, max.y),
			std::min(min.z, max.z),
			std::max(min.x, max.x),
			std::max(min.y, max.y),
			std::max(min.z, max.z)));

		OGRE_LOCK_MUTEX(meshBufferMutex)

		mMeshBuffer.push_back(
			MeshPairPtr(new MeshPair(
			mesh, MeshDataPairPtr(
			new MeshDataPair(patchMeshLoader, aabbPtr)))));

		OGRE_THREAD_NOTIFY_ALL(meshBufferSync)
	}

	Ogre::WorkQueue::Response * PatchMeshLoaderQueue::handleRequest(const Ogre::WorkQueue::Request * req, const Ogre::WorkQueue * srcQ)
	{
		Ogre::Timer timer;

		// Start infinite loop generating patches as they are added to the queue
		// Can probably be improved by using a CONDITION, to avoid busy waiting
		while (true)
		{
			{
				OGRE_LOCK_MUTEX(abortMutex)
				if (mAbort)
				{
					return 0;
				}
			}

			{
				// Move data from buffer to queue
				OGRE_LOCK_MUTEX_NAMED(meshBufferMutex, meshBufferMutexLock)

				while (mMeshBuffer.empty())
				{
					OGRE_THREAD_WAIT(meshBufferSync, meshBufferMutex, meshBufferMutexLock)
					OGRE_LOCK_MUTEX(abortMutex)
					if (mAbort)
					{
						return 0;
					}
				}

				OGRE_LOCK_MUTEX_NAMED(queueMutex, meshQueueMutexLock)
				while (!mMeshBuffer.empty())
				{
					mMeshQueue.push_back(mMeshBuffer.front());
					mMeshBuffer.pop_front();
				}
			}

			updatePrepMeshSet();

			OGRE_LOCK_MUTEX(queueMutex)

			// Resort every second
			if (timer.getMilliseconds() > 1000)
			{
				Ogre::Vector3 camPos;
				{
					OGRE_LOCK_MUTEX(cameraPosMutex)
					camPos = mCameraPos;
				}
				mMeshQueue.sort(Sorter(this, camPos));
				timer.reset();
			}

			while (mMeshQueue.size() > 0)
			{

				MeshPairPtr meshPairPtr = mMeshQueue.front();
				mMeshQueue.pop_front();
				Ogre::MeshPtr mesh = meshPairPtr->first;
				PatchMeshLoader * meshLoader = meshPairPtr->second->first;

				mesh->prepare(true);

				OGRE_LOCK_MUTEX(abortMutex)
				if (mAbort)
				{
					return 0;
				}
			}
		}

		return 0;
	}

	void PatchMeshLoaderQueue::handleResponse(const Ogre::WorkQueue::Response * res, const Ogre::WorkQueue * srcQ)
	{
	}

	Ogre::String PatchMeshLoaderQueue::getChannelName()
	{
		return "PatchMeshLoaderQueueChannel";
	}

	void PatchMeshLoaderQueue::setAbort()
	{
		OGRE_LOCK_MUTEX(abortMutex)
		mAbort = true;
		OGRE_THREAD_NOTIFY_ALL(meshBufferSync)
	}

	void PatchMeshLoaderQueue::abortPrepareMesh(Ogre::MeshPtr mesh)
	{
		// First, check to see if it's still waiting to be prepared
		{
			OGRE_LOCK_MUTEX(queueMutex)
			MeshQueue::iterator meshQueueIter = mMeshQueue.begin();
			while (meshQueueIter != mMeshQueue.end())
			{
				if ((*meshQueueIter)->first == mesh)
				{
					mMeshQueue.erase(meshQueueIter);
					return;
				}

				meshQueueIter++;
			}
		}

		// Not in the queue, tell resourceQueue to abort it
		OGRE_LOCK_MUTEX(prepMeshMutex)
		PrepMeshSet::iterator prepMeshIter = mPrepMeshSet.begin();
		while (prepMeshIter != mPrepMeshSet.end())
		{
			Ogre::MeshPtr _mesh = (*prepMeshIter)->first;
			if (_mesh == mesh)
			{
				Ogre::BackgroundProcessTicket ticket = (*prepMeshIter)->second;
				Ogre::ResourceBackgroundQueue::getSingleton().abortRequest(ticket);
				mPrepMeshSet.erase(prepMeshIter);
				return;
			}

			prepMeshIter++;
		}
	}

	void PatchMeshLoaderQueue::updatePrepMeshSet()
	{
		OGRE_LOCK_MUTEX(prepMeshMutex)
		PrepMeshSet::iterator prepMeshSetIter = mPrepMeshSet.begin();
		while (prepMeshSetIter != mPrepMeshSet.end())
		{
			Ogre::MeshPtr mesh = (*prepMeshSetIter)->first;

			if (mesh->isPrepared())
			{
				mPrepMeshSet.erase(prepMeshSetIter++);
			}
		}
	}
}