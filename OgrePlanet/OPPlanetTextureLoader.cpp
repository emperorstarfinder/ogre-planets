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

#include "OPPlanetTextureLoader.h"

namespace OgrePlanet
{
	PlanetTextureLoader::PlanetTextureLoader(DataSource * dataSource,
		int quads,
		Ogre::Vector3 & min,
		Ogre::Vector3 & max,
		Ogre::Real baseRadius,
		Ogre::Real scalingFactor) :
	HeightDataResourceLoader(dataSource, quads, min, max, 1, boost::shared_array<Ogre::Real>(new Ogre::Real[(quads+1) * (quads+1)])),
		mBaseRadius(baseRadius),
		mScalingFactor(scalingFactor),
		mColorDeeps(0.0, 0.0, 128.0/255.0),
		mColorShallow(0.0, 0.0, 255.0/255.0),
		mColorShore(0.0, 128.0/255.0, 255.0/255.0),
		mColorSand(240.0/255.0, 240.0/255.0, 64.0/255.0),
		mColorGrass(32.0/255.0, 160.0/255.0, 0.0),
		mColorDirt(224.0/255.0, 224.0/255.0, 0.0),
		mColorRock(128.0/255.0, 128.0/255.0, 128.0/255.0),
		mColorSnow(255.0/255.0, 255.0/255.0, 255.0/255.0)
	{
	}

	void PlanetTextureLoader::loadResource(Ogre::Resource *resource)
	{
		Ogre::Texture * texturePtr = static_cast<Ogre::Texture *>(resource);

		Ogre::Image::Box lockBox(0, 0, 0, texturePtr->getWidth(), texturePtr->getHeight(), texturePtr->getDepth());
		Ogre::PixelBox pixelBox = texturePtr->getBuffer()->lock(lockBox, Ogre::HardwareBuffer::HBL_DISCARD);
		Ogre::uint32 * pDest = static_cast<Ogre::uint32 *>(pixelBox.data);

		for (int y = 0; y < (mQuads + 1); y++)
		{
			for (int x = 0; x < (mQuads + 1); x++)
			{
				int index = (mQuads + 3) * (y + 1) + (x + 1);
				int nextXIndex = (mQuads + 3) * (y + 1) + (x + 2);
				int nextYIndex = (mQuads + 3) * (y + 2) + (x + 1);
				//pos = unitSpherePos[index] * (mBaseRadius + height[index] * mScalingFactor);

				int diffuseSlice = 0;
				int normalSlice = pixelBox.slicePitch;
				int row = y * pixelBox.rowPitch;
				Ogre::Real h = mData[index];
				Ogre::ColourValue lowColor;
				Ogre::ColourValue highColor;
				Ogre::Real lerpV;

				//if (h < -0.25)
				//{
				//	lowColor = mColorDeeps;
				//	highColor = mColorShallow;
				//	lerpV = (h + 1.0) / 0.75;
				//}
				//else if (h < -0.05)
				//{
				//	lowColor = mColorShallow;
				//	highColor = mColorShore;
				//	lerpV = (h + 0.25) / 0.2;
				//}
				//else if (h < 0.0)
				//{
				//	lowColor = mColorShore;
				//	highColor = mColorShore;
				//	lerpV = (h + 0.05) / 0.05;
				//}
				//else if (h < 0.05)
				if (h < 0.00)
				{
					lowColor = mColorDeeps;
					highColor = mColorDeeps;
					lerpV = 0.0;
				}
				else
				{
					lowColor = mColorSand;
					highColor = mColorSand;
					lerpV = 0.0;
				}
				/*
				if (h < 0.00)
				{
					lowColor = mColorSand;
					highColor = mColorSand;
					lerpV = 0.0;
				}
				else if (h < 0.05)
				{
					lowColor = mColorSand;
					highColor = mColorGrass;
					lerpV = h / 0.05;
				}
				else if (h < 0.375)
				{
					lowColor = mColorGrass;
					highColor = mColorDirt;
					lerpV = (h - 0.05) / 0.37;
				}
				else if (h < 0.5)
				{
					lowColor = mColorDirt;
					highColor = mColorRock;
					lerpV = (h - 0.375) / 0.125;
				}
				else if (h < 0.75)
				{
					lowColor = mColorRock;
					highColor = mColorSnow;
					lerpV = (h - 0.5) / 0.25;
				}
				else
				{
					lowColor = mColorSnow;
					highColor = mColorSnow;
					lerpV = 1.0;
				}
				*/

				Ogre::ColourValue color = lerpV * highColor + (1.0 - lerpV) * lowColor;
				Ogre::PixelUtil::packColour(color, texturePtr->getFormat(), &(pDest[diffuseSlice + row + x]));

				Ogre::Vector3 thisPos = (mBaseRadius + mScalingFactor * h) * mUnitSpherePos[index];
				Ogre::Vector3 nextXPos = (mBaseRadius + mScalingFactor * mData[nextXIndex]) * mUnitSpherePos[nextXIndex];
				Ogre::Vector3 nextYPos = (mBaseRadius + mScalingFactor * mData[nextYIndex]) * mUnitSpherePos[nextYIndex];

				Ogre::Vector3 normal = (nextYPos - thisPos).crossProduct(nextXPos - thisPos).normalisedCopy();
				Ogre::Vector3 bakedNormal = (normal + Ogre::Vector3::UNIT_SCALE) / 2.0;
				Ogre::ColourValue normalColor = Ogre::ColourValue(bakedNormal.x, bakedNormal.y, bakedNormal.z);
				//normalColor.a = (height[index] <= 0 ? 1.0 : 0.0); // bake specular map into normal map alpha channel
				Ogre::PixelUtil::packColour(normalColor, texturePtr->getFormat(), &(pDest[normalSlice + row + x]));
			}
		}

		texturePtr->getBuffer()->unlock();
	}
}