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
	float3 normal   : NORMAL;
	float4 texCoord	: TEXCOORD0;
	float4 interpolatedPosition : TEXCOORD1;
	float3 interpolatedNormal : TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float2 texCoord  : TEXCOORD0;
	float3 positionOS : TEXCOORD1;
	float3 normal   : TEXCOORD2;
	float height    : TEXCOORD3;
};

struct PS_INPUT
{
	float2 texCoord  : TEXCOORD0;
	float4 positionOS  : TEXCOORD1;
	float3 normal    : TEXCOORD2;
	float height     : TEXCOORD3;
};

float2 convertTexCoord(float2 texCoord, int sample)
{
	float2 myTexCoord = frac(texCoord) * (255.0 / 256.0) + 0.5 / 256.0;
	int x = fmod((float)sample,4.0);
	int y = 3 - (sample / 4);
	return float2(0.25 * x + 0.25 * myTexCoord.x, 0.25 * y + 0.25 * myTexCoord.y);
}

VS_OUTPUT main_vp(VS_INPUT input,
		uniform float4x4 worldViewProj,
		uniform float4x4 world,
		uniform float time,
		uniform float baseRadius,
		uniform float4 textureSize,
		uniform float4 patchCreationTime,
		uniform float4 stitch,
		uniform float4 patchCenter,
		uniform float farClipDistance)
{
	VS_OUTPUT output;
	
	//float lerpVal = saturate((time - patchCreationTime.x) / 2500.0);
	
	//input.position = lerp(input.interpolatedPosition, input.position, lerpVal);
	//input.normal = normalize(lerp(input.interpolatedNormal, input.normal, lerpVal));
	
	output.normal = input.normal;
	if (stitch.x > 0.5 && input.texCoord.z < 0.25) {
		output.normal = input.interpolatedNormal;
	} else if (stitch.y > 0.5 && input.texCoord.z > 0.75) {
		output.normal = input.interpolatedNormal;
	} else if (stitch.z > 0.5 && input.texCoord.w < 0.25) {
		output.normal = input.interpolatedNormal;
	} else if (stitch.w > 0.5 && input.texCoord.w > 0.75) {
		output.normal = input.interpolatedNormal;
	}

	output.positionOS = patchCenter.xyz + input.position.xyz;
	output.height = length(output.positionOS) - baseRadius;
	
	output.position = mul(worldViewProj, input.position);

	// Manage tex coords	
	float pixelWidth = 1.0/textureSize.x;
	float offset = 0.5 * pixelWidth;
	output.texCoord.x = (input.texCoord.x*pixelWidth)*(textureSize.x-1)+offset;
	output.texCoord.y = (input.texCoord.y*pixelWidth)*(textureSize.y-1)+offset;
		
	float C = 1.0;
	output.position.z = log(C*output.position.z + 1) / log(C*farClipDistance + 1) * output.position.w;
	
	return output;
}

float4 main_fp(PS_INPUT input,
		uniform float3 globalAmbient,
		uniform float3 lightColor,
		uniform float4 lightPosition,
		uniform float4 viewPosition,
		uniform float atmosphereRadius,
		uniform float planetRadius,
		uniform float scaleHeight,
		uniform float atmosphereDensity,
		uniform float3 Ka,
		uniform float3 Kd) : COLOR
{
	float3 N = normalize(input.normal);
	float3 L = normalize(lightPosition.xyz);
	float NdotL = dot(N, L);
	//float3 surfaceColor = tex3D(surfaceTexture, float3(input.texCoord.x, input.texCoord.y, 0.25)).rgb;
	
	float3 surfaceColor = float3(0.0, 0.5, 0.0);
	float height = length(input.positionOS.xyz) - planetRadius;
	float slope = saturate(dot(N, normalize(input.positionOS.xyz)));

	// Grass grows from sea level
	// At height 0 (km), mountain starts to appear
	// At height 3 (km), no grass grows
	float mountain = height / 3;
	
	if (mountain >= pow(slope, 50)) {
		surfaceColor = float3(0.5, 0.5, 0.5);
	}
	
	// Snow starts to appear at height 4 (km)
	// At height 6 (km) there is always snow
	float snow = (height - 4) / 2;
	
	// snow starts to appear on the flattest terrain
	if (snow > (1 - pow(slope, 5))) {
		surfaceColor = float3(1.0, 1.0, 1.0);
	}

	float4 c;
	c.rgb = (Ka * globalAmbient + Kd*lightColor * max(NdotL, 0.0)) * surfaceColor;
	c.a = 1.0;

	//float f = atmosphereSurface(input.positionOS, viewPosition.xyz, lightPosition.xyz, atmosphereRadius, planetRadius, scaleHeight, atmosphereDensity, 10);
	//return lerp(float4(0.75, 0.75, 1.0, 1.0), c, f);
	
	return c;
}