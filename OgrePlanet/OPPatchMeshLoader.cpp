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

#include "OPPatchMeshLoader.h"
#include "OPPatch.h"
#include "OPStitching.h"

namespace OgrePlanet
{
	Ogre::HardwareIndexBufferSharedPtr PatchMeshLoader::indexBuffer[16];

	void PatchMeshLoader::init(int quads) {
		int maxTriangles = 2 * quads * quads;

		for (int i = 0; i < 16; i++) {
			int triangles = maxTriangles;
			switch(i) {
				case STITCHING_NONE:
					break;
				case STITCHING_W:
				case STITCHING_N:
				case STITCHING_E:
				case STITCHING_S:
					triangles -= (quads / 2);
					break;
				case STITCHING_WN:
				case STITCHING_WE:
				case STITCHING_WS:
				case STITCHING_NE:
				case STITCHING_NS:
				case STITCHING_ES:
					triangles -= (2 * (quads / 2));
					break;
				case STITCHING_WNE:
				case STITCHING_WNS:
				case STITCHING_WES:
				case STITCHING_NES:
					triangles -= (3 * (quads / 2));
					break;
				case STITCHING_WNES:
					triangles -= (4 * (quads / 2));
					break;
			}

			indexBuffer[i] = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT,
				3 * triangles,
				Ogre::HardwareBuffer::HBU_WRITE_ONLY);
			unsigned short * pIndices = static_cast<unsigned short *>(indexBuffer[i]->lock(Ogre::HardwareBuffer::HBL_DISCARD));

			for (int y = 0; y < quads; y++) {
				for (int x = 0; x < quads; x++) {
					if (
						i == STITCHING_NONE ||	// No stitching
							(x != 0 && x != (quads-1) && y != 0 && y != (quads-1)) || // interior quad, no stitching
							(i == STITCHING_W && x > 0) ||
							(i == STITCHING_N && y > 0) ||
							(i == STITCHING_E && x < (quads-1)) ||
							(i == STITCHING_S && y < (quads-1)) ||
							(i == STITCHING_WN && x > 0 && y > 0) ||
							(i == STITCHING_WE && x > 0 && x < (quads-1)) ||
							(i == STITCHING_WS && x > 0 && y < (quads-1)) ||
							(i == STITCHING_NE && x < (quads-1) && y > 0) ||
							(i == STITCHING_NS && y > 0 && y < (quads-1)) ||
							(i == STITCHING_ES && x < (quads-1) && y < (quads-1)) ||
							(i == STITCHING_WNE && x > 0 && x < (quads-1) && y > 0) ||
							(i == STITCHING_WNS && x > 0 && y > 0 && y < (quads-1)) ||
							(i == STITCHING_WES && x > 0 && x < (quads-1) && y < (quads-1)) ||
							(i == STITCHING_NES && x < (quads-1) && y > 0 && y < (quads-1)))
					{
						*pIndices++ = y * (quads + 1) + x;
						*pIndices++ = (y + 1) * (quads + 1) + x;
						*pIndices++ = y * (quads + 1) + x + 1;
						*pIndices++ = (y + 1) * (quads + 1) + x;
						*pIndices++ = (y + 1) * (quads + 1) + x + 1;
						*pIndices++ = y * (quads + 1) + x + 1;
					}
					else if (i == STITCHING_W)
					{
						// x must be 0 here
						if (y % 2 == 0) {
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 2) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
						} else {
							// Only one triangle
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
						}
					}
					else if (i == STITCHING_N)
					{
						// y must be 0 here
						if (x % 2 == 0) {
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 2;
						} else {
							// Only one triangle
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
						}
					}
					else if (i == STITCHING_E)
					{
						// x must be (quads-1) here
						if (y % 2 == 0) {
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = y * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 2) * (quads + 1) + x + 1;
						} else {
							// Only one triangle
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
						}
					}
					else if (i == STITCHING_S)
					{
						// y must be (quads-1) here
						if (x % 2 == 0) {
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = y * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
							*pIndices++ = (y + 1) * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 2;
						} else {
							// Only one triangle
							*pIndices++ = y * (quads + 1) + x;
							*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							*pIndices++ = y * (quads + 1) + x + 1;
						}
					}
					else if (i == STITCHING_WN) {
						if (y == 0) {
							if (x == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WE) {
						if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads - 1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WS) {
						if (y == (quads-1)) {
							if (x == 0) {
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_NE) {
						if (y == 0) {
							if (x == (quads-1)) {
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_NS) {
						if (y == 0) {
							if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (y == (quads-1)) {
							if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_ES) {
						if (y == (quads-1)) {
							if (x == (quads-1)) {
								// Do nothing
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WNE) {
						if (y == 0) {
							if (x == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else if (x == (quads-1)) {
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WNS) {
						if (y == 0) {
							if (x == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (y == (quads-1)) {
							if (x == 0) {
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WES) {
						if (y == (quads-1)) {
							if (x == 0) {
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else if (x == (quads-1)) {
								// Do nothing
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_NES) {
						if (y == 0) {
							if (x == (quads-1)) {
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (y == (quads-1)) {
							if (x == (quads-1)) {
								// Do nothing
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
					else if (i == STITCHING_WNES) {
						if (y == 0) {
							if (x == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else if (x == (quads-1)) {
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (y == (quads-1)) {
							if (x == 0) {
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else if (x == (quads-1)) {
								// Do nothing
							} else if (x % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 2;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == 0) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
							}
						} else if (x == (quads-1)) {
							if (y % 2 == 0) {
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = y * (quads + 1) + x + 1;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 2) * (quads + 1) + x + 1;
							} else {
								// Only one triangle
								*pIndices++ = y * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x;
								*pIndices++ = (y + 1) * (quads + 1) + x + 1;
							}
						}
					}
				}
			}

			indexBuffer[i]->unlock();
		}
	}

	void PatchMeshLoader::cleanup()
	{
		for (int i = 0; i < 16; i++) {
			indexBuffer[i].setNull();
		}
	}
	
	PatchMeshLoader::PatchMeshLoader(DataSource * dataSource,
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
		boost::shared_array<Ogre::Real> parentData,
		int position) :
	HeightDataResourceLoader(dataSource, quads, min, max, 2, data, parentData, position),
		mTexXMin(texXMin),
		mTexXMax(texXMax),
		mTexYMin(texYMin),
		mTexYMax(texYMax),
		mBaseRadius(baseRadius),
		mScalingFactor(scalingFactor),
		mAABB(AABB)
	{
	}

	void PatchMeshLoader::prepareResource(Ogre::Resource * resource)
	{
		// First make sure height data is available
		HeightDataResourceLoader::prepareResource(resource);

		vertexPosition = boost::shared_array<Ogre::Vector3>(new Ogre::Vector3[(mQuads + 2*mPadding + 1) * (mQuads + 2*mPadding + 1)]); // Need extra padding to calculate normals
		vertexNormal = boost::shared_array<Ogre::Vector3>(new Ogre::Vector3[(mQuads + 1) * (mQuads + 1)]);
		textureCoordinate = boost::shared_array<Ogre::Vector2>(new Ogre::Vector2[(mQuads + 1) * (mQuads + 1)]);
		interpolatedVertexPosition = boost::shared_array<Ogre::Vector3>(new Ogre::Vector3[(mQuads + 1) * (mQuads + 1)]);
		interpolatedVertexNormal = boost::shared_array<Ogre::Vector3>(new Ogre::Vector3[(mQuads + 1) * (mQuads + 1)]);

		Ogre::Vector3 minBounds(MAXINT, MAXINT, MAXINT);
		Ogre::Vector3 maxBounds(-MAXINT, -MAXINT, -MAXINT);

		// Now calculate vertex positions (with padding) in planet space
		for (int y = 0; y < (mQuads + 2*mPadding + 1); y++)
		{
			for (int x = 0; x < (mQuads + 2*mPadding + 1); x++)
			{
				int index = (mQuads + 2*mPadding + 1) * y + x;
				vertexPosition[index] = mUnitSpherePos[index] * (mBaseRadius + mData[index] * mScalingFactor);

				minBounds.x = std::min(minBounds.x, vertexPosition[index].x);
				minBounds.y = std::min(minBounds.y, vertexPosition[index].y);
				minBounds.z = std::min(minBounds.z, vertexPosition[index].z);
				maxBounds.x = std::max(maxBounds.x, vertexPosition[index].x);
				maxBounds.y = std::max(maxBounds.y, vertexPosition[index].y);
				maxBounds.z = std::max(maxBounds.z, vertexPosition[index].z);
			}
		}

		// Transform vertex positions to object space (i.e. centered around origin)

		mAABB.setExtents(minBounds, maxBounds);
		mCenter = mAABB.getCenter();
		minBounds -= mCenter;
		maxBounds -= mCenter;

		for (int y = 0; y < (mQuads + 2*mPadding + 1); y++)
		{
			for (int x = 0; x < (mQuads + 2*mPadding + 1); x++)
			{
				int index = (mQuads + 2*mPadding + 1) * y + x;
				vertexPosition[index] -= mCenter;
			}
		}

		mAABB.setExtents(minBounds, maxBounds);

		// Calculate vertex normals, texture coordinates and interpolated positions
		for (int y = 0; y < (mQuads + 1); y++)
		{
			for (int x = 0; x < (mQuads + 1); x++)
			{
				int index = (mQuads + 1) * y + x;

				// 6-connected, with triangle-area correction
				int pIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding);
				int pNextXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding + 1);
				int pNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 1) + (x + mPadding);
				int pPrevXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding - 1);
				int pPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 1) + (x + mPadding);
				int pNextXPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 1) + (x + mPadding + 1);
				int pPrevXNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 1) + (x + mPadding - 1);

				Ogre::Vector3 thisVertex = vertexPosition[pIndex];
				Ogre::Vector3 nextXVertex = vertexPosition[pNextXIndex];
				Ogre::Vector3 nextYVertex = vertexPosition[pNextYIndex];
				Ogre::Vector3 prevXVertex = vertexPosition[pPrevXIndex];
				Ogre::Vector3 prevYVertex = vertexPosition[pPrevYIndex];
				Ogre::Vector3 nextXPrevYVertex = vertexPosition[pNextXPrevYIndex];
				Ogre::Vector3 prevXNextYVertex = vertexPosition[pPrevXNextYIndex];

				Ogre::Vector3 n1 = (nextXVertex - thisVertex).crossProduct(nextXPrevYVertex - thisVertex);
				Ogre::Vector3 n2 = (nextXPrevYVertex - thisVertex).crossProduct(prevYVertex - thisVertex);
				Ogre::Vector3 n3 = (prevYVertex - thisVertex).crossProduct(prevXVertex - thisVertex);
				Ogre::Vector3 n4 = (prevXVertex - thisVertex).crossProduct(prevXNextYVertex - thisVertex);
				Ogre::Vector3 n5 = (prevXNextYVertex - thisVertex).crossProduct(nextYVertex - thisVertex);
				Ogre::Vector3 n6 = (nextYVertex - thisVertex).crossProduct(nextXVertex - thisVertex);

				vertexNormal[index] = (n1 + n2 + n3 + n4 + n5 + n6).normalisedCopy();

				//// 4-connected, no triangle-area correction
				//int pIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding);
				//int pNextXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding + 1);
				//int pNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 1) + (x + mPadding);
				//int pPrevXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding - 1);
				//int pPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 1) + (x + mPadding);
				//int pNextXPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 1) + (x + mPadding + 1);
				//int pPrevXNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 1) + (x + mPadding - 1);

				//Ogre::Vector3 thisVertex = vertexPosition[pIndex];
				//Ogre::Vector3 nextXVertex = vertexPosition[pNextXIndex];
				//Ogre::Vector3 nextYVertex = vertexPosition[pNextYIndex];
				//Ogre::Vector3 prevXVertex = vertexPosition[pPrevXIndex];
				//Ogre::Vector3 prevYVertex = vertexPosition[pPrevYIndex];
				//Ogre::Vector3 nextXPrevYVertex = vertexPosition[pNextXPrevYIndex];
				//Ogre::Vector3 prevXNextYVertex = vertexPosition[pPrevXNextYIndex];

				//Ogre::Vector3 n1 = (nextXVertex - thisVertex).crossProduct(prevYVertex - thisVertex).normalisedCopy();
				//Ogre::Vector3 n2 = (prevYVertex - thisVertex).crossProduct(prevXVertex - thisVertex).normalisedCopy();
				//Ogre::Vector3 n3 = (prevXVertex - thisVertex).crossProduct(nextYVertex - thisVertex).normalisedCopy();
				//Ogre::Vector3 n4 = (nextYVertex - thisVertex).crossProduct(nextXVertex - thisVertex).normalisedCopy();

				//vertexNormal[index] = (n1 + n2 + n3 + n4).normalisedCopy();

				// Texture coordinates (this is global planet texture coordinates)
				Ogre::Real jx = ((Ogre::Real) x)/mQuads;
				Ogre::Real jy = ((Ogre::Real) y)/mQuads;

				textureCoordinate[index].x = (1 - jx) * mTexXMin + jx * mTexXMax;
				textureCoordinate[index].y = (1 - jy) * mTexYMin + jy * mTexYMax;

				// Interpolated position
				if ((x % 2 != 0) && (y % 2 != 0))
				{
					// Odd x and y coordinate, this vertex doesn't exist in parent
					// and must be geomorphed.

					interpolatedVertexPosition[index] = 0.5 * nextXPrevYVertex + 0.5 * prevXNextYVertex;
				}
				else if (x % 2 != 0)
				{
					// Odd x coordinate, this vertex doesn't exist in parent
					// and must be geomorphed
					interpolatedVertexPosition[index] = 0.5 * nextXVertex + 0.5 * prevXVertex;
				}
				else if (y % 2 != 0)
				{
					// Odd y coordinate, this vertex doesn't exist in parent
					// and must be geomorphed
					interpolatedVertexPosition[index] = 0.5 * nextYVertex + 0.5 * prevYVertex;
				}
				else
				{
					// This vertex exists in parent, no morphing required
					interpolatedVertexPosition[index] = thisVertex;
				}
			}
		}

		// Calculate interpolated normals
		// First we calculate normals for those vertices that also exist in
		// parent LOD. We do this because these are then needed to calculate
		// the interpolated normals for those vertices that do not exist
		// in parent LOD level.
		for (int y = 0; y < (mQuads + 1); y++)
		{
			for (int x = 0; x < (mQuads + 1); x++)
			{
				if ((x % 2 == 0) && (y % 2 == 0))
				{
					int index = (mQuads + 1) * y + x;

					// 6-connected, with triangle-area correction
					int pIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding);
					int pNextXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding + 2);
					int pNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 2) + (x + mPadding);
					int pPrevXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding - 2);
					int pPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 2) + (x + mPadding);
					int pNextXPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 2) + (x + mPadding + 2);
					int pPrevXNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 2) + (x + mPadding - 2);

					Ogre::Vector3 thisVertex = vertexPosition[pIndex];
					Ogre::Vector3 nextXVertex = vertexPosition[pNextXIndex];
					Ogre::Vector3 nextYVertex = vertexPosition[pNextYIndex];
					Ogre::Vector3 prevXVertex = vertexPosition[pPrevXIndex];
					Ogre::Vector3 prevYVertex = vertexPosition[pPrevYIndex];
					Ogre::Vector3 nextXPrevYVertex = vertexPosition[pNextXPrevYIndex];
					Ogre::Vector3 prevXNextYVertex = vertexPosition[pPrevXNextYIndex];

					Ogre::Vector3 n1 = (nextXVertex - thisVertex).crossProduct(nextXPrevYVertex - thisVertex);
					Ogre::Vector3 n2 = (nextXPrevYVertex - thisVertex).crossProduct(prevYVertex - thisVertex);
					Ogre::Vector3 n3 = (prevYVertex - thisVertex).crossProduct(prevXVertex - thisVertex);
					Ogre::Vector3 n4 = (prevXVertex - thisVertex).crossProduct(prevXNextYVertex - thisVertex);
					Ogre::Vector3 n5 = (prevXNextYVertex - thisVertex).crossProduct(nextYVertex - thisVertex);
					Ogre::Vector3 n6 = (nextYVertex - thisVertex).crossProduct(nextXVertex - thisVertex);

					interpolatedVertexNormal[index] = (n1 + n2 + n3 + n4 + n5 + n6).normalisedCopy();

					//// 4-connected, no triangle-area correction
					//int pIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding);
					//int pNextXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding + 2);
					//int pNextYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding + 2) + (x + mPadding);
					//int pPrevXIndex = (mQuads + 2*mPadding + 1) * (y + mPadding) + (x + mPadding - 2);
					//int pPrevYIndex = (mQuads + 2*mPadding + 1) * (y + mPadding - 2) + (x + mPadding);

					//Ogre::Vector3 thisVertex = vertexPosition[pIndex];
					//Ogre::Vector3 nextXVertex = vertexPosition[pNextXIndex];
					//Ogre::Vector3 nextYVertex = vertexPosition[pNextYIndex];
					//Ogre::Vector3 prevXVertex = vertexPosition[pPrevXIndex];
					//Ogre::Vector3 prevYVertex = vertexPosition[pPrevYIndex];

					//Ogre::Vector3 n1 = (nextXVertex - thisVertex).crossProduct(prevYVertex - thisVertex).normalisedCopy();
					//Ogre::Vector3 n2 = (prevYVertex - thisVertex).crossProduct(prevXVertex - thisVertex).normalisedCopy();
					//Ogre::Vector3 n3 = (prevXVertex - thisVertex).crossProduct(nextYVertex - thisVertex).normalisedCopy();
					//Ogre::Vector3 n4 = (nextYVertex - thisVertex).crossProduct(nextXVertex - thisVertex).normalisedCopy();

					//interpolatedVertexNormal[index] = (n1 + n2 + n3 + n4).normalisedCopy();
				}
			}
		}

		// Now we can calculate the interpolated normals for those
		// vertices that do not exist in parent LOD level.
		for (int y = 0; y < (mQuads + 1); y++)
		{
			for (int x = 0; x < (mQuads + 1); x++)
			{
				int index = (mQuads + 1) * y + x;

				if ((x % 2 != 0) && (y % 2 != 0))
				{
					int prevXNextYIndex = (mQuads + 1) * (y + 1) + (x - 1);
					int nextXPrevYIndex = (mQuads + 1) * (y - 1) + (x + 1);
					interpolatedVertexNormal[index] = (0.5 * interpolatedVertexNormal[prevXNextYIndex] + 0.5 * (interpolatedVertexNormal[nextXPrevYIndex])).normalisedCopy();
				}
				else if (x % 2 != 0)
				{
					int prevXIndex = (mQuads + 1) * y + (x - 1);
					int nextXIndex = (mQuads + 1) * y + (x + 1);
					interpolatedVertexNormal[index] = (0.5 * interpolatedVertexNormal[prevXIndex] + 0.5 * (interpolatedVertexNormal[nextXIndex])).normalisedCopy();
				}
				else if (y % 2 != 0)
				{
					int prevYIndex = (mQuads + 1) * (y - 1) + x;
					int nextYIndex = (mQuads + 1) * (y + 1) + x;
					interpolatedVertexNormal[index] = (0.5 * interpolatedVertexNormal[prevYIndex] + 0.5 * (interpolatedVertexNormal[nextYIndex])).normalisedCopy();
				}
			}
		}
	}

	void PatchMeshLoader::loadResource(Ogre::Resource *resource)
	{
		Ogre::Mesh * meshPtr = static_cast<Ogre::Mesh *>(resource);
		Ogre::SubMesh * subMeshPtr = meshPtr->createSubMesh();

		meshPtr->sharedVertexData = new Ogre::VertexData();
		Ogre::VertexData * vertexData = meshPtr->sharedVertexData;

		Ogre::VertexDeclaration * vertexDecl = vertexData->vertexDeclaration;
		size_t currOffset = 0;
		vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
		currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT4, Ogre::VES_TEXTURE_COORDINATES, 0);
		currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);

		// Interpolated vertex position used by geomorphing
		vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES, 1);
		currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		// Interpolated normal used by geomorphing
		vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES, 2);
		currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		vertexData->vertexCount = (mQuads + 1) * (mQuads + 1);
		Ogre::HardwareVertexBufferSharedPtr vBuf =
			Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			vertexDecl->getVertexSize(0),
			vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		Ogre::VertexBufferBinding * binding = vertexData->vertexBufferBinding;
		binding->setBinding(0, vBuf);
		float * pVertex = static_cast<float *>(vBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

		subMeshPtr->indexData->indexCount = indexBuffer[0]->getNumIndexes();
		subMeshPtr->indexData->indexBuffer = indexBuffer[0];

		/*
		subMeshPtr->indexData->indexCount = 6 * mQuads * mQuads;
		subMeshPtr->indexData->indexBuffer =
			Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_16BIT,
			subMeshPtr->indexData->indexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		Ogre::HardwareIndexBufferSharedPtr iBuf = subMeshPtr->indexData->indexBuffer;
		unsigned short * pIndices = static_cast<unsigned short *>(iBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
		*/

		for (int y = 0; y < (mQuads + 1); y++)
		{
			for (int x = 0; x < (mQuads + 1); x++)
			{
				int index1 = (mQuads + 2 * mPadding + 1) * (y + mPadding) + (x + mPadding);
				int index2 = (mQuads + 1) * y + x;

				// Vertex pos
				*pVertex++ = vertexPosition[index1].x;
				*pVertex++ = vertexPosition[index1].y;
				*pVertex++ = vertexPosition[index1].z;

				// Vertex normal
				*pVertex++ = vertexNormal[index2].x;
				*pVertex++ = vertexNormal[index2].y;
				*pVertex++ = vertexNormal[index2].z;

				// Texture coordinates (this is global planet texture coordinates)
				*pVertex++ = textureCoordinate[index2].x;
				*pVertex++ = textureCoordinate[index2].y;
				*pVertex++ = (x == 0 ? 0.0 : (x == mQuads ? 1.0 : 0.5));
				*pVertex++ = (y == 0 ? 0.0 : (y == mQuads ? 1.0 : 0.5));

				// Interpolated position
				*pVertex++ = interpolatedVertexPosition[index2].x;
				*pVertex++ = interpolatedVertexPosition[index2].y;
				*pVertex++ = interpolatedVertexPosition[index2].z;

				// Interpolated normal
				*pVertex++ = interpolatedVertexNormal[index2].x;
				*pVertex++ = interpolatedVertexNormal[index2].y;
				*pVertex++ = interpolatedVertexNormal[index2].z;

				/*
				if (x < mQuads && y < mQuads)
				{
					*pIndices++ = y * (mQuads + 1) + x;
					*pIndices++ = (y + 1) * (mQuads + 1) + x;
					*pIndices++ = y * (mQuads + 1) + x + 1;
					*pIndices++ = (y + 1) * (mQuads + 1) + x;
					*pIndices++ = (y + 1) * (mQuads + 1) + x + 1;
					*pIndices++ = y * (mQuads + 1) + x + 1;
				}
				*/
			}
		}

		// Done with the data arrays, release them to conserve memory
		mUnitSpherePos = boost::shared_array<Ogre::Vector3>(0);
		vertexPosition = boost::shared_array<Ogre::Vector3>(0);
		vertexNormal = boost::shared_array<Ogre::Vector3>(0);
		textureCoordinate = boost::shared_array<Ogre::Vector2>(0);
		interpolatedVertexPosition = boost::shared_array<Ogre::Vector3>(0);
		interpolatedVertexNormal = boost::shared_array<Ogre::Vector3>(0);

		vBuf->unlock();
		//iBuf->unlock();
		subMeshPtr->useSharedVertices = true;

		meshPtr->_setBounds(mAABB);
		meshPtr->_setBoundingSphereRadius(mAABB.getHalfSize().length());
	}

	Ogre::Real PatchMeshLoader::getBaseRadius()
	{
		return mBaseRadius;
	}
}