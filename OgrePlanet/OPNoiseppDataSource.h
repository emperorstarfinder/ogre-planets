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

#ifndef NOISEPPDATASOURCE_H
#define NOISEPPDATASOURCE_H

#include "OPDataSource.h"

#include "noisepp/core/Noise.h"

namespace OgrePlanet
{
	class NoiseppDataSource : public DataSource
	{
	public:
		NoiseppDataSource();
		~NoiseppDataSource();
		Ogre::Real getValue(const Ogre::Vector3 &position);

	protected:
		OGRE_AUTO_MUTEX

	private:
		noisepp::Pipeline3D * pipeline;
		noisepp::Cache * cache;
		noisepp::PipelineElement3D * element;

		noisepp::PerlinModule mContinents;
		noisepp::SelectModule mContinentSelect;
		noisepp::ConstantModule mOcean;
		noisepp::PerlinModule mMountainDefinition;
		noisepp::ScalePointModule mMountainDefinitionScalePoint;
		noisepp::SelectModule mMountainSelect;
		noisepp::ScaleBiasModule mMountainSelectScaleBias;
		noisepp::RidgedMultiModule mMountains;
		noisepp::ScaleBiasModule mMountainsScaleBias;
		noisepp::ScalePointModule mMountainsScalePoint;
		noisepp::BillowModule mLowlands;
		noisepp::ScaleBiasModule mLowlandsScaleBias;
		noisepp::ScalePointModule mLowlandsScalePoint;
	};
}

#endif // NOISEPPDATASOURCE_H