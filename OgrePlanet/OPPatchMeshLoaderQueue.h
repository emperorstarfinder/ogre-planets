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

#ifndef PATCHMESHLOADERQUEUE_H
#define PATCHMESHLOADERQUEUE_H

#include "OPPatchMeshLoader.h"

#include "OPUtil.h"

#include <Ogre.h>

#include <boost/shared_ptr.hpp>

namespace OgrePlanet
{
	typedef boost::shared_ptr<Ogre::AxisAlignedBox> AxisAlignedBoxPtr;
	typedef std::pair<PatchMeshLoader *, AxisAlignedBoxPtr> MeshDataPair;
	typedef boost::shared_ptr<MeshDataPair> MeshDataPairPtr;
	typedef std::pair<Ogre::MeshPtr, MeshDataPairPtr> MeshPair;
	typedef boost::shared_ptr<MeshPair> MeshPairPtr;
	typedef std::list<MeshPairPtr> MeshQueue;
	typedef std::pair<Ogre::MeshPtr, Ogre::BackgroundProcessTicket> PrepMeshPair;
	typedef boost::shared_ptr<PrepMeshPair> PrepMeshPairPtr;
	typedef std::set<PrepMeshPairPtr> PrepMeshSet;

	class PatchMeshLoaderQueue :
		public Ogre::Singleton<PatchMeshLoaderQueue>,
		public Ogre::WorkQueue::RequestHandler,
		public Ogre::WorkQueue::ResponseHandler
	{
	public:
		static PatchMeshLoaderQueue & getSingleton();
		static PatchMeshLoaderQueue * getSingletonPtr();

		PatchMeshLoaderQueue();

		void setCameraPosition(const Ogre::Vector3 & pos);
		void prepareMesh(Ogre::MeshPtr mesh, PatchMeshLoader * patchMeshLoader);

		Ogre::WorkQueue::Response * handleRequest(const Ogre::WorkQueue::Request * req,
			const Ogre::WorkQueue * srcQ);
		void handleResponse(const Ogre::WorkQueue::Response * res, const Ogre::WorkQueue * srcQ);
		Ogre::String getChannelName();
		void abortPrepareMesh(Ogre::MeshPtr mesh);
		void setAbort();

	private:
		friend class Sorter;

		int mMaxDepth;
		bool mAbort;
		Ogre::Vector3 mCameraPos;

		OGRE_MUTEX(abortMutex)
		OGRE_MUTEX(prepMeshMutex)
		OGRE_MUTEX(queueMutex)
		OGRE_MUTEX(cameraPosMutex)
		OGRE_MUTEX(meshBufferMutex)

		OGRE_THREAD_SYNCHRONISER(meshBufferSync)

		MeshQueue mMeshQueue;
		MeshQueue mMeshBuffer;
		PrepMeshSet mPrepMeshSet;

		void updatePrepMeshSet();

		class Sorter
		{
		private:
			PatchMeshLoaderQueue * mPatchMeshLoaderQueue;
			Ogre::Vector3 mCamPos;
		public:
			Sorter(PatchMeshLoaderQueue * q, const Ogre::Vector3 & camPos) :
				mPatchMeshLoaderQueue(q),
				mCamPos(camPos)
			{}
			bool operator()(const MeshPairPtr & lhs, const MeshPairPtr & rhs)
			{
				return Util::squaredDistance(mCamPos, *(lhs->second->second)) <
					Util::squaredDistance(mCamPos, *(lhs->second->second));
					//mCamPos.squaredDistance(*(lhs->second->second)) <
					//mCamPos.squaredDistance(*(rhs->second->second));
			}
		};
	};

	
}

#endif // PATCHMESHLOADERQUEUE_H