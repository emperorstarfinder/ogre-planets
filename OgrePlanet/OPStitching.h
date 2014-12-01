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

#ifndef STITCHING_H
#define STITCHING_H

#include <Ogre.h>

namespace OgrePlanet
{
	enum Stitching {
		STITCHING_NONE = 0,
		STITCHING_W = 1,
		STITCHING_N = 2,
		STITCHING_WN = 3,
		STITCHING_E = 4,
		STITCHING_WE = 5,
		STITCHING_NE = 6,
		STITCHING_WNE = 7,
		STITCHING_S = 8,
		STITCHING_WS = 9,
		STITCHING_NS = 10,
		STITCHING_WNS = 11,
		STITCHING_ES = 12,
		STITCHING_WES = 13,
		STITCHING_NES = 14,
		STITCHING_WNES = 15,
		
	};
}

#endif // STITCHING_H