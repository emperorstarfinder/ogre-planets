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

#include "OPHeightDataResourceLoader.h"

namespace OgrePlanet
{
	HeightDataResourceLoader::HeightDataResourceLoader(DataSource * dataSource,
		int quads,
		Ogre::Vector3 & min,
		Ogre::Vector3 & max,
		int padding,
		boost::shared_array<Ogre::Real> data,
		boost::shared_array<Ogre::Real> parentData,
		int position) : 
	mDataSource(dataSource),
		mQuads(quads),
		mMin(min),
		mMax(max),
		mPadding(padding),
		mData(data),
		mParentData(parentData),
		mPosition(position)
	{
	}

	HeightDataResourceLoader::~HeightDataResourceLoader()
	{
	}

	void HeightDataResourceLoader::prepareResource(Ogre::Resource *resource)
	{
		assert(
			(mMin.x == 1.0 && mMax.x == 1.0) ||
			(mMin.x == -1.0 && mMax.x == -1.0) ||
			(mMin.y == 1.0 && mMax.y == 1.0) ||
			(mMin.y == -1.0 && mMax.y == -1.0) ||
			(mMin.z == 1.0 && mMax.z == 1.0) ||
			(mMin.z == -1.0 && mMax.z == -1.0));

		// Create a height map, with one extra padding in each direction,
		// so we can calculate normals later (if needed)
		mUnitSpherePos = boost::shared_array<Ogre::Vector3>(new Ogre::Vector3[(mQuads + 2*mPadding +1) * (mQuads + 2*mPadding + 1)]);
		//mData.reset(new Ogre::Real[(mQuads + 2*mPadding + 1) * (mQuads + 2*mPadding + 1)]);

		Ogre::Vector3 pos;

		// Use "texture coordinates"
		// _xPos is where U axis changes
		Ogre::Real *_xPos;
		// _yPos is where V axis changes
		Ogre::Real *_yPos;
		// _zPos is where the normal would be
		Ogre::Real *_zPos;

		Ogre::Vector2 startPos;
		Ogre::Vector2 endPos;

		int a;
		int b;
		int c;

		if (mMin.x == 1.0 && mMax.x == 1.0)
		{
			// Patch on right side
			// On right side, T coordinate lies in Z axis
			_xPos = &(pos.z);
			// U coordinate lies in Y axis
			_yPos = &(pos.y);
			// X axis is normal
			_zPos = &(pos.x);
			startPos.x = mMin.z;
			startPos.y = mMin.y;
			endPos.x = mMax.z;
			endPos.y = mMax.y;
			a = -1;
			b = -1;
			c = 1;
		}
		if (mMin.x == -1.0 && mMax.x == -1.0)
		{
			// Patch on left side
			_xPos = &(pos.z);
			_yPos = &(pos.y);
			_zPos = &(pos.x);
			startPos.x = mMin.z;
			startPos.y = mMin.y;
			endPos.x = mMax.z;
			endPos.y = mMax.y;
			a = 1;
			b = -1;
			c = -1;
		}
		else if (mMin.y == 1.0 && mMax.y == 1.0)
		{
			// Patch on top side
			_xPos = &(pos.x);
			_yPos = &(pos.z);
			_zPos = &(pos.y);
			startPos.x = mMin.x;
			startPos.y = mMin.z;
			endPos.x = mMax.x;
			endPos.y = mMax.z;
			a = 1;
			b = 1;
			c = 1;
		}
		else if (mMin.y == -1.0 && mMax.y == -1.0)
		{
			// Patch on bottom side
			_xPos = &(pos.x);
			_yPos = &(pos.z);
			_zPos = &(pos.y);
			startPos.x = mMin.x;
			startPos.y = mMin.z;
			endPos.x = mMax.x;
			endPos.y = mMax.z;
			a = 1;
			b = -1;
			c = -1;
		}
		else if (mMin.z == 1.0 && mMax.z == 1.0)
		{
			// Patch on front side
			_xPos = &(pos.x);
			_yPos = &(pos.y);
			_zPos = &(pos.z);
			startPos.x = mMin.x;
			startPos.y = mMin.y;
			endPos.x = mMax.x;
			endPos.y = mMax.y;
			a = 1;
			b = -1;
			c = 1;
		}
		else if (mMin.z == -1.0 && mMax.z == -1.0)
		{
			// Patch on back side
			_xPos = &(pos.x);
			_yPos = &(pos.y);
			_zPos = &(pos.z);
			startPos.x = mMin.x;
			startPos.y = mMin.y;
			endPos.x = mMax.x;
			endPos.y = mMax.y;
			a = -1;
			b = -1;
			c = -1;
		}

		Ogre::Real &xPos = (*_xPos);
		Ogre::Real &yPos = (*_yPos);
		Ogre::Real &zPos = (*_zPos);

		Ogre::Vector3 minBounds(MAXINT, MAXINT, MAXINT);
		Ogre::Vector3 maxBounds(-MAXINT, -MAXINT, -MAXINT);

		if (mDataSource->getValuesSupported())
		{
			mData = mDataSource->getValues(mQuads, mPadding, mMin, mMax);
		}
		else
		{
			for (int y = 0-mPadding; y <= (mQuads + mPadding); y++)
			{
				for (int x = 0-mPadding; x <= (mQuads + mPadding); x++)
				{
					int index = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding);

					xPos = (startPos.x + (endPos.x - startPos.x) * (((double) x)/mQuads));
					yPos = (startPos.y + (endPos.y - startPos.y) * (((double) y)/mQuads));
					zPos = c;
					pos.normalise();
					mUnitSpherePos[index] = pos;

					if (mParentData.get() != 0 &&
						(x % 2) == 0 &&
						(y % 2) == 0)
					{
						int parentX = x / 2 + (mPosition % 2 == 1 ? mQuads / 2 : 0);
						int parentY = y / 2 + (mPosition >= 2 ? mQuads / 2 : 0);
						int parentIndex = (mQuads + 2*mPadding + 1) * (parentY + mPadding) + (parentX + mPadding);
						Ogre::Real height = mParentData[parentIndex];
						mData[index] = height;
					}
					else
					{
						Ogre::Real height = mDataSource->getValue(pos);
						mData[index] = height;
					}
				}
			}
		}
	}

	const Ogre::Vector3 & HeightDataResourceLoader::getMin()
	{
		return mMin;
	}

	const Ogre::Vector3 & HeightDataResourceLoader::getMax()
	{
		return mMax;
	}

	boost::shared_array<Ogre::Real> HeightDataResourceLoader::getData()
	{
		return mData;
	}
}