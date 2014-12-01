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

#include "OPRawDataSource.h"

namespace OgrePlanet
{
	RawDataSource::RawDataSource(const Ogre::String &fileName, int width, int height) :
		mWidth(width),
		mHeight(height)
	{
		//f.open(fileName.c_str(), std::ios::binary | std::ios::in);
	}

	RawDataSource::~RawDataSource()
	{
		//f.close();
	}

	Ogre::Real RawDataSource::getValue(const Ogre::Vector3 &position)
	{
		//Ogre::Vector3 sphericalPos = cartesianToSpherical(position);

		//// Scale theta and phi to [0, 1] range
		//Ogre::Real x = (sphericalPos.x + Ogre::Math::PI) / Ogre::Math::TWO_PI;
		//Ogre::Real y = (sphericalPos.y + Ogre::Math::HALF_PI) / Ogre::Math::PI;

		//// Multiply index by 2, since we're interested in WORDs.
		//unsigned long index = 2 * (((unsigned long) (mWidth * mHeight * y)) + ((unsigned long) (mWidth * x)));
		//f.seekg(index, std::ios_base::beg);

		//short data = 0;
		//char d;

		//f.read((char *) &d, sizeof(d));
		//data += d;
		//data <<= 8;
		//f.read((char *) &d, sizeof(d));
		//data += d;

		//return ((Ogre::Real) data)/8800;

		Ogre::Vector3 sphericalPos = cartesianToSpherical(position);

		// Scale theta and phi to [0, 1] range
		Ogre::Real x = (sphericalPos.x + Ogre::Math::PI) / Ogre::Math::TWO_PI;
		Ogre::Real y = (sphericalPos.y + Ogre::Math::HALF_PI) / Ogre::Math::PI;

		Ogre::Image heightMap;
		heightMap.load("earthHeightmap.jpg", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::PixelBox pb = heightMap.getPixelBox();
		BYTE p = ((BYTE *)(pb.data))[pb.getWidth() * int(pb.getHeight() * y) + int(pb.getWidth() * x)];

		return p;
	}

	Ogre::Vector3 RawDataSource::cartesianToSpherical(const Ogre::Vector3 &position)
	{
		Ogre::Real r = position.length();
		Ogre::Real s = Ogre::Math::Sqrt(position.x * position.x + position.z * position.z);
		Ogre::Real theta = Ogre::Math::ACos(position.y/r).valueRadians();
		Ogre::Real phi =
			(position.z >= 0 ? Ogre::Math::ASin(position.x / s).valueRadians() :
				Ogre::Math::PI - Ogre::Math::ASin(position.x / s).valueRadians());

		return Ogre::Vector3(theta, phi, r);
	}
}