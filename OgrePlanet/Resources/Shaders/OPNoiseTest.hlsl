// Copyright (c) 2010 Anders Lingfors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "OPNoise.hlsl"

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main_vp(VS_INPUT input, uniform int quads, uniform int padding)
{
	VS_OUTPUT output;

	float pixelWidth = 1.0/quads;
	float offset = 0.5 * pixelWidth;

	input.position.xy = sign(input.position.xy);
	output.position = float4(input.position.xy, 0, 1);

	output.texCoord.x = 0.5 * (1 + output.position.x * (1 + 2 * offset));
	output.texCoord.y = 0.5 * (1 - output.position.y);

	output.texCoord.x = (output.texCoord.x*pixelWidth)*(quads-1)+offset;
	output.texCoord.y = (output.texCoord.y*pixelWidth)*(quads-1)+offset;
	
	return output;
}

float4 main_fp(PS_INPUT input, uniform int octaves, uniform uint face, uniform float3 min, uniform float3 max) : COLOR
{
	float3 pos = float3(0, 0, 0);

	switch (face)
	{
	case 0:
		// Right face

		//return -1;

		pos.x = 1;
		pos.z = lerp(min.z, max.z, input.texCoord.x);
		pos.y = lerp(min.y, max.y, input.texCoord.y);
		break;
	case 1:
		// Left face

		//return -1;

		pos.x = -1;
		pos.z = lerp(min.z, max.z, input.texCoord.x);
		pos.y = lerp(min.y, max.y, input.texCoord.y);
		break;
	case 2:
		// Top

		return -1;

		pos.y = 1;
		pos.x = lerp(min.x, max.x, input.texCoord.x);
		pos.z = lerp(min.z, max.z, input.texCoord.y);
		break;
	case 3:
		// Bottom

		//return -1;

		pos.y = -1;
		pos.x = lerp(min.x, max.x, input.texCoord.x);
		pos.z = lerp(min.z, max.z, input.texCoord.y);
		break;
	case 4:
		// Front

		//return -1;

		pos.z = 1;
		pos.x = lerp(min.x, max.x, input.texCoord.x);
		pos.y = lerp(min.y, max.y, input.texCoord.y);
		break;
	case 5:
		// Back

		//return -1;

		pos.z = -1;
		pos.x = lerp(min.x, max.x, input.texCoord.x);
		pos.y = lerp(min.y, max.y, input.texCoord.y);
		break;
	}

	return 1;

	normalize(pos);
	return 0.5 * (fBm(10 * pos, octaves) + 1);
}