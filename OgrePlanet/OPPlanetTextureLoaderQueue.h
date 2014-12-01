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

#ifndef PLANETTEXTURELOADERQUEUE_H
#define PLANETTEXTURELOADERQUEUE_H

#include "OPPlanetTextureLoader.h"

#include <Ogre.h>

namespace OgrePlanet
{
	typedef std::list<Ogre::TexturePtr> TextureQueue;

	class PlanetTextureLoaderQueue :
		public Ogre::Singleton<PlanetTextureLoaderQueue>,
		public Ogre::WorkQueue::RequestHandler,
		public Ogre::WorkQueue::ResponseHandler
	{
	public:
		static PlanetTextureLoaderQueue & getSingleton();
		static PlanetTextureLoaderQueue * getSingletonPtr();

		PlanetTextureLoaderQueue();

		void prepareTexture(Ogre::TexturePtr texture);

		Ogre::WorkQueue::Response * handleRequest(const Ogre::WorkQueue::Request * req, const Ogre::WorkQueue * srcQ);
		void handleResponse(const Ogre::WorkQueue::Response * res, const Ogre::WorkQueue * srcQ);
		Ogre::String getChannelName();
		void setAbort();

	private:
		int mMaxDepth;
		bool mAbort;

		OGRE_MUTEX(abortMutex)
		OGRE_MUTEX(queueMutex)
		OGRE_MUTEX(bufferMutex)

		OGRE_THREAD_SYNCHRONISER(bufferSync)

		TextureQueue mTextureQueue;
		TextureQueue mTextureBuffer;
	};
}

#endif // PLANETTEXTURELOADERQUEUE_H