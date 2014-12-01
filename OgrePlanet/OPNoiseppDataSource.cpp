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

#include "OPNoiseppDataSource.h"

#include "noisepp/utils/NoiseUtils.h"

namespace OgrePlanet
{
	NoiseppDataSource::NoiseppDataSource()
	{
		mOcean.setValue(-1.0);

		mMountains.setOctaveCount(12);

		mMountainsScaleBias.setSourceModule(0, mMountains);
		mMountainsScaleBias.setScale(0.8);
		mMountainsScaleBias.setBias(0.2);
		mMountainsScalePoint.setSourceModule(0, mMountainsScaleBias);
		mMountainsScalePoint.setScaleX(250);
		mMountainsScalePoint.setScaleY(250);
		mMountainsScalePoint.setScaleZ(250);

		mLowlands.setOctaveCount(7);
		mLowlandsScaleBias.setSourceModule(0, mLowlands);
		mLowlandsScaleBias.setScale(0.2);
		mLowlandsScaleBias.setBias(-0.8);
		mLowlandsScalePoint.setSourceModule(0, mLowlandsScaleBias);
		mLowlandsScalePoint.setScaleX(250);
		mLowlandsScalePoint.setScaleY(250);
		mLowlandsScalePoint.setScaleZ(250);

		mMountainDefinition.setOctaveCount(12);
		mMountainDefinitionScalePoint.setSourceModule(0, mMountainDefinition);
		mMountainDefinitionScalePoint.setScaleX(10);
		mMountainDefinitionScalePoint.setScaleY(10);
		mMountainDefinitionScalePoint.setScaleZ(10);

		mMountainSelect.setControlModule(mMountainDefinitionScalePoint);
		mMountainSelect.setSourceModule(0, mLowlandsScalePoint);
		mMountainSelect.setSourceModule(1, mMountainsScalePoint);
		mMountainSelect.setEdgeFalloff(0.1);
		mMountainSelect.setLowerBound(0.5);
		mMountainSelectScaleBias.setSourceModule(0, mMountainSelect);
		mMountainSelectScaleBias.setScale(0.5);
		mMountainSelectScaleBias.setBias(0.5);

		mContinents.setOctaveCount(12);
		mContinents.setFrequency(1.0);
		mContinents.setLacunarity(2.0);
		mContinents.setPersistence(0.625);

		mContinentSelect.setControlModule(mContinents);
		mContinentSelect.setSourceModule(0, mOcean);
		mContinentSelect.setSourceModule(1, mMountainSelectScaleBias);
		mContinentSelect.setLowerBound(0.0);
		mContinentSelect.setEdgeFalloff(0.1);

		pipeline = new noisepp::Pipeline3D;
		noisepp::ElementID id = mContinentSelect.addToPipeline(pipeline);
		element = pipeline->getElement(id);
		cache = pipeline->createCache();
	}

	NoiseppDataSource::~NoiseppDataSource()
	{
		pipeline->freeCache(cache);
		delete pipeline;
	}

	Ogre::Real NoiseppDataSource::getValue(const Ogre::Vector3 &position)
	{
		OGRE_LOCK_AUTO_MUTEX
		pipeline->cleanCache(cache);
		return element->getValue(position.x, position.y, position.z, cache);
	};

}
