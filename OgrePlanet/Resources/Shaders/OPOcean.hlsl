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

static const float planetRadius = 6371.0;
static const float maxHeight = 8.848;
static const float atmosphereRadius = 6391.0;

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texCoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float3 normal   : TEXCOORD0;
	float3 positionObjectSpace : TEXCOORD1;
};

struct PS_INPUT
{
	float3 normal   : TEXCOORD0;
	float3 positionObjectSpace : TEXCOORD1;
};

VS_OUTPUT main_vp(
		VS_INPUT input,

		uniform float4x4 worldViewProj,
		uniform float baseRadius,
		uniform float farClipDistance)
{
	VS_OUTPUT output;

	output.normal = normalize(input.normal);
	output.position = mul(worldViewProj, input.position);
	output.positionObjectSpace = input.position.xyz;
	
	float C = 1.0;
	output.position.z = log(C*output.position.z + 1) / log(C*farClipDistance + 1) * output.position.w;

	return output;
}

float4 main_fp(PS_INPUT input,
	
		uniform float3 globalAmbient,
		uniform float3 lightColor,
		uniform float3 lightPosition,
		uniform float3 viewPosition,
		uniform float time,
		uniform float3 Ka,
		uniform float3 Kd,
		uniform float3 Ks,
		uniform float shininess,
		uniform float atmosphereRadius,
		uniform float planetRadius,
		uniform float scaleHeight,
		uniform float atmosphereDensity,
		uniform float4 patchCenter) : COLOR
{
	float4 color;

	float3 N = normalize(input.normal);
	float3 L = normalize(lightPosition); // Direction to light source
	float NdotL = dot(N, L);
	float3 R = normalize(2 * N * NdotL - L); // Light reflection direction
	float3 V = normalize(viewPosition-input.positionObjectSpace); // Direction from lit surface to eye
	float RdotV = dot(R, V);
	color.rgb = (Ka * globalAmbient + Kd*lightColor * max(NdotL, 0.0)) * float3(0.0, 0.25, 0.25) +
		Ks * lightColor * pow(max(RdotV, 0.0), shininess);

	color.rgb = atmosphere2(color.rgb, patchCenter.xyz + input.positionObjectSpace.xyz, patchCenter.xyz + viewPosition.xyz, lightPosition, planetRadius, atmosphereRadius);

	color.a = 1.0;

	return color;
}