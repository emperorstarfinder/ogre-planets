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

float4x4 worldViewProj;
float4 lightPosition;
float4 viewPosition;
float planetRadius;
float atmosphereRadius;
float scaleHeight;
float atmosphereDensity;
float4 patchCenter;

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float3 positionOS : TEXCOORD0;
};

struct PS_INPUT 
{
	float3 positionOS : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
};


VS_OUTPUT vs_main(VS_INPUT input,
		uniform float farClipDistance)
{
	VS_OUTPUT output;
	
	// object space to projection space
	output.position = mul(worldViewProj, input.position);

	// keep the rest in object space
	output.positionOS = patchCenter.xyz + input.position.xyz;

	float C = 1.0;
	output.position.z = log(C*output.position.z + 1) / log(C*farClipDistance + 1) * output.position.w;

	return output;
}

float4 ps_main(PS_INPUT input) : COLOR0
{
	float3 sunDir = normalize(lightPosition.xyz);

	float4 color;
	color.rgb = atmosphere2(float3(0.0, 0.0, 0.0), input.positionOS.xyz, patchCenter.xyz + viewPosition.xyz, sunDir, planetRadius, atmosphereRadius);
	color.a = 1.0;
	//color = float4(1.0, 1.0, 1.0, 1.0);
	return color;
}