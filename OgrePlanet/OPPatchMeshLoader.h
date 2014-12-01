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

#ifndef PATCHMESHLOADER_H
#define PATCHMESHLOADER_H

#include "OPDataSource.h"
#include "OPPatchMeshLoaderDestroyer.h"
#include "OPHeightDataResourceLoader.h"

namespace OgrePlanet
{
	class Patch;

	class PatchMeshLoader : public HeightDataResourceLoader
	{
	public:
		static Ogre::HardwareIndexBufferSharedPtr indexBuffer[16];

		static void init(int quads);
		static void cleanup();

		PatchMeshLoader(DataSource * dataSource,
			int quads,
			Ogre::Vector3 & min,
			Ogre::Vector3 & max,
			Ogre::Real texXMin,
			Ogre::Real texXMax,
			Ogre::Real texYMin,
			Ogre::Real texYMax,
			Ogre::Real baseRadius,
			Ogre::Real scalingFactor,
			Ogre::AxisAlignedBox & AABB,
			boost::shared_array<Ogre::Real> data,
			boost::shared_array<Ogre::Real> parentData = boost::shared_array<Ogre::Real>(),
			int position = 0);
		void prepareResource(Ogre::Resource * resource);
		void loadResource(Ogre::Resource * resource);
		Ogre::Real getBaseRadius();
		const Ogre::Vector3 & getCenter() { return mCenter; }

	protected:
		boost::shared_array<Ogre::Vector3> vertexPosition;
		boost::shared_array<Ogre::Vector3> vertexNormal;
		boost::shared_array<Ogre::Vector2> textureCoordinate;
		boost::shared_array<Ogre::Vector3> interpolatedVertexPosition;
		boost::shared_array<Ogre::Vector3> interpolatedVertexNormal;

	private:
		const Ogre::Real mBaseRadius;
		const Ogre::Real mScalingFactor;
		Ogre::AxisAlignedBox & mAABB;
		Ogre::Real mTexXMin;
		Ogre::Real mTexXMax;
		Ogre::Real mTexYMin;
		Ogre::Real mTexYMax;
		Ogre::Vector3 mCenter;
	};
}

#endif // PATCHMESHLOADER_H
