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

#include "OPDEMDataSource.h"

#include <boost/lexical_cast.hpp>

namespace OgrePlanet
{
	Ogre::Real DEMDataSource::getValue(const Ogre::Vector3 &position)
	{
		Ogre::Radian phi, theta;
		convertToSpherical(position, phi, theta);
		return lookupValue(phi, theta);
	}

	void DEMDataSource::convertToSpherical(const Ogre::Vector3 & position, Ogre::Radian & phi, Ogre::Radian & theta)
	{
		Ogre::Real r = position.length();
		phi = Ogre::Math::ATan2(position.y, position.x);
		theta = Ogre::Math::ACos(position.z / r);
	}

	Ogre::Real DEMDataSource::lookupValue(const Ogre::Radian & phi, const Ogre::Radian & theta)
	{
		Ogre::String longitude;
		Ogre::String latitude;

		Ogre::Real longitudeDeg;
		Ogre::Real latitudeDeg;

		bool longitudeFini = false;
		if (phi.valueRadians() < Ogre::Math::PI)
		{
			// East of meridian
			if (phi.valueDegrees() < 20)
			{
				// Longitudes less than 20 degrees east of meridian
				// is found in the "w020" files
				longitude = "w020";
				longitudeFini = true;
			}
			else
			{
				longitude = "e";
				longitudeDeg = phi.valueDegrees();
				longitudeDeg -= 20;
			}
		}
		else
		{
			// West of meridian
			longitude = "w";
			longitudeDeg = 360 - phi.valueDegrees();
			longitudeDeg += 20;
		}

		if (!longitudeFini)
		{
			longitudeDeg /= 40;
			longitudeDeg *= 40;
			longitudeDeg += 20;
			if (longitudeDeg < 100)
			{
				longitude += "0";
			}
			longitude += boost::lexical_cast<Ogre::String, Ogre::Real>(longitudeDeg);
		}

		bool latitudeFini = false;
		if (theta.valueRadians() < Ogre::Math::HALF_PI)
		{
			// North of equator
			latitude = "n";
			latitudeDeg = 90 - phi.valueDegrees();
			latitudeDeg += 10;
			latitudeDeg /= 50;
			latitudeDeg *= 50;
			latitudeDeg += 40;
		}
		else
		{
			// South of equator
			if (theta.valueDegrees() > -10)
			{
				// Latitudes less than 10 degrees south of the equator
				// belongs to the "n40" files
				latitude = "n40";
				latitudeFini = true;
			}
			latitude = "s";
			latitudeDeg = theta.valueDegrees() - 90;
			latitudeDeg -= 10;
			latitudeDeg /= 50;
			latitudeDeg *= 50;
			latitudeDeg += 10;
		}

		if (!latitudeFini)
		{
			latitude += boost::lexical_cast<Ogre::String, Ogre::Real>(latitudeDeg);
		}

		return 0;
	}
}