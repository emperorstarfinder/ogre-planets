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

#ifndef GPUNOISEDATASOURCE_H
#define GPUNOISEDATASOURCE_H

#include "OPDataSource.h"

namespace OgrePlanet
{
	class GpuNoiseDataSource : public DataSource
	{
	public:
		Ogre::Real getValue(const Ogre::Vector3 &position);
		bool getValuesSupported() { return true; }
		boost::shared_array<Ogre::Real> getValues(int quads, int padding, const Ogre::Vector3 & min, const Ogre::Vector3 & max);
	protected:
		OGRE_AUTO_MUTEX
	private:
		class CompositorListener : public Ogre::CompositorInstance::Listener
		{
		public:
			CompositorListener(int quads, int padding, const Ogre::Vector3 & min, const Ogre::Vector3 & max);
			void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr & material);
		private:
			int mQuads;
			int mPadding;
			Ogre::Vector3 mMin;
			Ogre::Vector3 mMax;
		};
	};
}

#endif // GPUNOISEDATASOURCE_H
