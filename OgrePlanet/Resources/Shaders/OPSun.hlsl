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
	float3 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float3 texCoord : TEXCOORD0;
};

VS_OUTPUT main_vp(VS_INPUT input, uniform float4x4 worldViewProj)
{
	VS_OUTPUT output;

	output.position = mul(worldViewProj, input.position);

	output.texCoord.x = input.position.x;
	output.texCoord.y = input.position.y;
	output.texCoord.z = input.position.z;
	
	return output;
}

float4 main_fp(PS_INPUT input, uniform float time) : COLOR
{
	float a = 0.5 * (ridgedmf(float4(0.001 * input.texCoord, time), 8) + 1);
	float b = 0.5 * (turbulence(float4(0.001 * input.texCoord, time), 8) + 1);
	float c = 0.5 * (10 * fBm(float4(0.001 * input.texCoord, time), 1) + 1);
	//float c = 0.5 * (ridgedmf(float4(0.001 * input.texCoord, time), 8) + 1);
	float3 red = float3(1.0, 0.0, 0.0);
	float3 yellow = float3(1.0, 1.0, 0.0);
	return float4(lerp(red, yellow, lerp(a, b, c)), 1.0);
}