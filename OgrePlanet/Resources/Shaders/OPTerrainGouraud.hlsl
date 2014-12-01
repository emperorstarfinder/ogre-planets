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

#include "functions.hlsl"

sampler3D surfaceTexture : register(s0);

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 texCoord	: TEXCOORD0;
	float4 interpolatedPosition : TEXCOORD1;
	float3 interpolatedNormal : TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float shade   : TEXCOORD0;
};

struct PS_INPUT
{
	float shade : TEXCOORD0;
};

VS_OUTPUT main_vp(VS_INPUT input,
		uniform float4x4 worldViewProj,
		uniform float4 stitch,
		uniform float farClipDistance,
		uniform float3 lightPosition)
{
	VS_OUTPUT output;
	
	if (stitch.x > 0.5 && input.texCoord.z < 0.25) {
		input.normal = input.interpolatedNormal;
	} else if (stitch.y > 0.5 && input.texCoord.z > 0.75) {
		input.normal = input.interpolatedNormal;
	} else if (stitch.z > 0.5 && input.texCoord.w < 0.25) {
		input.normal = input.interpolatedNormal;
	} else if (stitch.w > 0.5 && input.texCoord.w > 0.75) {
		input.normal = input.interpolatedNormal;
	}
	input.normal = normalize(input.normal);

	output.position = mul(worldViewProj, input.position);
	output.shade = dot(input.normal, normalize(lightPosition));

	float C = 1.0;
	output.position.z = log(C*output.position.z + 1) / log(C*farClipDistance + 1) * output.position.w;
	
	return output;
}

float4 main_fp(PS_INPUT input) : COLOR
{
	float4 color;
	//color.rgb = input.shade.xxx;
	//color.a = 1.0;
	color = float4(1.0, 1.0, 1.0, 1.0);
	
	return color;
}