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

#include "OPUtil.h"

namespace OgrePlanet
{
	Ogre::Real Util::squaredDistance(const Ogre::Vector3 & position, const Ogre::AxisAlignedBox & AABB)
	{
		Ogre::Vector3 d(0.0, 0.0, 0.0);
		Ogre::Vector3 min = AABB.getMinimum();
		Ogre::Vector3 max = AABB.getMaximum();

		if (position.x < min.x)
		{
			d.x = min.x - position.x;
		}
		else if (position.x > max.x)
		{
			d.x = position.x - max.x;
		}

		if (position.y < min.y)
		{
			d.y = min.y - position.y;
		}
		else if (position.y > max.y)
		{
			d.y = position.y - max.y;
		}

		if (position.z < min.z)
		{
			d.z = min.z - position.z;
		}
		else if (position.z > max.z)
		{
			d.z = position.z - max.z;
		}

		return d.squaredLength();
	}

	Ogre::Real Util::distance(const Ogre::Vector3 & position, const Ogre::AxisAlignedBox & AABB)
	{
		return Ogre::Math::Sqrt(squaredDistance(position, AABB));
	}
}