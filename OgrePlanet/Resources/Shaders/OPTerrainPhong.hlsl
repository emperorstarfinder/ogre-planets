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
#include "OPNoise.hlsl"

sampler2D grassTexture : register(s2);
sampler2D rockTexture : register(s3);
sampler2D snowTexture : register(s4);

static const float planetRadius = 6371.0;
static const float maxHeight = 8.848;
static const float atmosphereRadius = 6391.0;

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
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 positionObjectSpace : TEXCOORD2;
};

struct PS_INPUT
{
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 positionObjectSpace : TEXCOORD2;
};

VS_OUTPUT main_vp(VS_INPUT input,
		uniform float4x4 worldViewProj,
		uniform float farClipDistance,
		uniform float4 stitch)
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
	output.normal = normalize(input.normal);

	output.position = mul(worldViewProj, input.position);
	output.positionObjectSpace = input.position.xyz;
	output.texCoord = input.texCoord.xy;

	float C = 1.0;
	output.position.z = log(C*output.position.z + 1) / log(C*farClipDistance + 1) * output.position.w;
	
	return output;
}

float4 main_fp(PS_INPUT input,
		uniform float3 viewPosition,
		uniform float3 lightPosition,
		uniform float4 patchCenter) : COLOR
{
	float3 normal = normalize(input.normal);
	float shade = saturate(dot(normal, normalize(lightPosition)));

	float3 grass = 0.25 * tex2D(grassTexture, input.texCoord).rgb
		+ 0.25 * tex2D(grassTexture, (1.0 / 8.0) * input.texCoord).rgb
		+ 0.25 * tex2D(grassTexture, (1.0 / (8.0*8.0)) * input.texCoord).rgb
		+ 0.25 * tex2D(grassTexture, (1.0 / (8.0*8.0*8.0)) * input.texCoord).rgb;
	float3 rock = 0.25 * tex2D(rockTexture, input.texCoord).rgb
		+ 0.25 * tex2D(rockTexture, (1.0 / 8.0) * input.texCoord).rgb
		+ 0.25 * tex2D(rockTexture, (1.0 / (8.0*8.0)) * input.texCoord).rgb
		+ 0.25 * tex2D(rockTexture, (1.0 / (8.0*8.0*8.0)) * input.texCoord).rgb;
	float3 snow = tex2D(snowTexture, input.texCoord).rgb;

	float4 color;
	color.rgb = grass;

	float height = length(patchCenter + input.positionObjectSpace) - planetRadius;
	float scaleHeight = saturate(height / maxHeight);
	float slope = degrees(acos(dot(normalize(patchCenter + input.positionObjectSpace), normal)));
	float mountainFactor = saturate((slope - 30.0 + scaleHeight * 90) / 10.0);
	color.rgb = lerp(color.rgb, rock, mountainFactor);

	float snowHeight = saturate(sign(scaleHeight + 0.1 * fBm(patchCenter + input.positionObjectSpace, 6) - 0.3));
	float snowFactor = snowHeight * (1.0 - sign(slope + 5 * fBm(patchCenter + input.positionObjectSpace + float3(100.0, 100.0, 100.0), 3) - 30));
	//float snowHeight = saturate(sign(scaleHeight - 0.3));
	//float snowFactor = snowHeight * (1.0 - sign(slope - 30));
	color.rgb = lerp(color.rgb, snow, snowFactor);

	color.rgb *= shade;

	color.rgb = atmosphere2(color.rgb, patchCenter + input.positionObjectSpace, patchCenter + viewPosition, lightPosition, planetRadius, atmosphereRadius);

	color.a = 1.0;
	
	return color;
}