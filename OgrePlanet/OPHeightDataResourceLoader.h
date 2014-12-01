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

#ifndef HEIGHTDATARESOURCELOADER_H
#define HEIGHTDATARESOURCELOADER_H

#include "OPDataSource.h"
#include "OPPatchMeshLoaderDestroyer.h"
#include <Ogre.h>
#include <boost/shared_array.hpp>

namespace OgrePlanet
{
	class HeightDataResourceLoader : public Ogre::ManualResourceLoader
	{
	public:
		HeightDataResourceLoader(DataSource * dataSource,
			int quads,
			Ogre::Vector3 & min,
			Ogre::Vector3 & max,
			int padding,
			boost::shared_array<Ogre::Real> data,
			boost::shared_array<Ogre::Real> parentData = boost::shared_array<Ogre::Real>(),
			int position = 0);
		virtual ~HeightDataResourceLoader() = 0;
		void prepareResource(Ogre::Resource * resource);
		const Ogre::Vector3 & getMin();
		const Ogre::Vector3 & getMax();
		boost::shared_array<Ogre::Real> getData();

	protected:
		boost::shared_array<Ogre::Vector3> mUnitSpherePos;
		boost::shared_array<Ogre::Real> mData;
		const int mQuads;
		const int mPadding;

	private:
		const Ogre::Vector3 & mMin;
		const Ogre::Vector3 & mMax;
		DataSource * mDataSource;
		boost::shared_array<Ogre::Real> mParentData;
		int mPosition;
	};
}

#endif // HEIGHTDATARESOURCELOADER_H
