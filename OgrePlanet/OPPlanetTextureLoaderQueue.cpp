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

#include "OPPlanetTextureLoaderQueue.h"

template<> OgrePlanet::PlanetTextureLoaderQueue* Ogre::Singleton<OgrePlanet::PlanetTextureLoaderQueue>::ms_Singleton = 0;

namespace OgrePlanet
{
	PlanetTextureLoaderQueue* PlanetTextureLoaderQueue::getSingletonPtr(void)
    {
        return ms_Singleton;
    }

	PlanetTextureLoaderQueue& PlanetTextureLoaderQueue::getSingleton(void)
    {
        assert( ms_Singleton );  return ( *ms_Singleton );
    }

	PlanetTextureLoaderQueue::PlanetTextureLoaderQueue() :
		mMaxDepth(-1),
		mAbort(false)
	{}

	void PlanetTextureLoaderQueue::prepareTexture(Ogre::TexturePtr texture)
	{
		OGRE_LOCK_MUTEX(bufferMutex)

		mTextureBuffer.push_back(texture);

		OGRE_THREAD_NOTIFY_ALL(bufferSync)
	}

	Ogre::WorkQueue::Response * PlanetTextureLoaderQueue::handleRequest(const Ogre::WorkQueue::Request * req, const Ogre::WorkQueue * srcQ)
	{
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
				OGRE_LOCK_MUTEX_NAMED(bufferMutex, bufferMutexLock)

				while (mTextureBuffer.empty())
				{
					OGRE_THREAD_WAIT(bufferSync, bufferMutex, bufferMutexLock)
					OGRE_LOCK_MUTEX(abortMutex)
					if (mAbort)
					{
						return 0;
					}
				}

				OGRE_LOCK_MUTEX_NAMED(queueMutex, queueMutexLock)
				while (!mTextureBuffer.empty())
				{
					mTextureQueue.push_back(mTextureBuffer.front());
					mTextureBuffer.pop_front();
				}
			}

			OGRE_LOCK_MUTEX(queueMutex)

			while (mTextureQueue.size() > 0)
			{

				Ogre::TexturePtr texture = mTextureQueue.front();
				mTextureQueue.pop_front();
				texture->prepare();

				OGRE_LOCK_MUTEX(abortMutex)
				if (mAbort)
				{
					return 0;
				}
			}
		}

		return 0;
	}

	void PlanetTextureLoaderQueue::handleResponse(const Ogre::WorkQueue::Response * res, const Ogre::WorkQueue * srcQ)
	{
	}

	Ogre::String PlanetTextureLoaderQueue::getChannelName()
	{
		return "PlanetTextureLoaderQueueChannel";
	}

	void PlanetTextureLoaderQueue::setAbort()
	{
		OGRE_LOCK_MUTEX(abortMutex)
		mAbort = true;
		OGRE_THREAD_NOTIFY_ALL(bufferSync)
	}
}