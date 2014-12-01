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

static const float PI = 3.14159265f;

// rayDir must be normalized
bool raySphereIntersect(float3 rayOrig, float3 rayDir, float r)
{
	float B = 2 * (rayOrig.x * rayDir.x + rayOrig.y * rayDir.y + rayOrig.z * rayDir.z);
	float C = rayOrig.x * rayOrig.x + rayOrig.y * rayOrig.y + rayOrig.z * rayOrig.z - r * r;

	float det = B*B - 4*C;
	if (det < 0.0) {
		return false;
	}

	return ((-B + sqrt(det)) > 0);
}

// rayDir must be normalized
float3 raySphereIntersectDistance(float3 rayOrig, float3 rayDir, float r)
{
	float3 returnVal = float3(0.0, 0.0, 0.0);

	float B = 2 * (rayOrig.x * rayDir.x + rayOrig.y * rayDir.y + rayOrig.z * rayDir.z);
	float C = rayOrig.x * rayOrig.x + rayOrig.y * rayOrig.y + rayOrig.z * rayOrig.z - r * r;

	float det = B*B - 4*C;
	if (det >= 0.0)
	{
		returnVal.x = 2.0;
		returnVal.y = (-B + sqrt(det)) / 2;
		returnVal.z = (-B - sqrt(det)) / 2;
	}

	return returnVal;
}

float orenNayarDiffuse(float3 light, float3 view, float3 normal, float roughness)
{
	float LdotN = dot(light, normal);

	float A = 1.0;
	float B = 0.0;

	if (roughness > 0.0)
	{
		float sigma2 = roughness * roughness;
		A = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
	
		float VdotN = dot(view, normal);
		float cosPhiDiff = dot(normalize(light - LdotN * normal), normalize(view - VdotN * normal));

		if (cosPhiDiff > 0)
		{
			float thetaI = acos(LdotN);
			float thetaR = acos(VdotN);

			float alpha = max(thetaI, thetaR);
			float beta = min(thetaI, thetaR);

			B = 0.45 * sigma2 / (sigma2 + 0.09);
			B *= cosPhiDiff * sin(alpha) * tan(beta) ;
		}
	}

	return max(LdotN, 0.0) * (A + B);
}

float3 objectSpaceToTangentSpace(float3 v, float3 normal, float3 tangent)
{
	float3 biTangent = normalize(cross(normal, tangent));
	float3x3 tbnMatrix = float3x3(tangent, biTangent, normal);
	return normalize(mul(tbnMatrix, v));
}

float3 bumpMapNormalTS(sampler2D BumpMap, float2 texCoord, float2 bumpMapTextureOffset, float heightScale)
{
	float uOff = bumpMapTextureOffset.x;
	float vOff = bumpMapTextureOffset.y;

	float b = tex2D(BumpMap, texCoord).x;
	float bu = tex2D(BumpMap, float2(texCoord.x + uOff, texCoord.y)).x;
	float bv = tex2D(BumpMap, float2(texCoord.x, texCoord.y - vOff)).x;

	// return normal in tangent space
	return normalize(cross(float3(uOff, 0, heightScale * uOff * (bu-b)), float3(0, vOff, heightScale * vOff * (bv-b))));
}

float bumpMapDiffuse(float3 lightTS, sampler2D BumpMap, float2 texCoord, float2 bumpMapTextureOffset, float heightScale)
{
	return max(dot(lightTS, bumpMapNormalTS(BumpMap, texCoord, bumpMapTextureOffset, heightScale)), 0);
}

float atmosphere(float3 fragmentPosition, float3 viewPosition, float3 lightPosition, float atmosphereRadius, float planetRadius, float scaleHeight, float atmosphereDensity, int samplePoints)
{
	float3 view = normalize(viewPosition - fragmentPosition);

	// Find the "real" point on the sphere
	float3 p = fragmentPosition + raySphereIntersectDistance(fragmentPosition, view, atmosphereRadius).z * view;

	// recalculate view
	view = normalize(viewPosition - p);

	float depth = min(raySphereIntersectDistance(p, view, atmosphereRadius).y, length(viewPosition - p));
	float f = 1.0;
	float stepLength = depth / samplePoints;
	float halfStepLength = stepLength / 2.0;
	float3 currentPosition = p;

	for (int i = 0; i < samplePoints; i++)
	{
		float3 light = normalize(lightPosition - currentPosition);

		float height = (length(currentPosition) - planetRadius) / (atmosphereRadius - planetRadius);
		float k1 = exp(-height / scaleHeight);
		height = (length(currentPosition + halfStepLength * view) - planetRadius) / (atmosphereRadius - planetRadius);
		float k2 = exp(-height / scaleHeight);
		height = (length(currentPosition + stepLength * view) - planetRadius) / (atmosphereRadius - planetRadius);
		float k3 = exp(-height / scaleHeight);

		float density = atmosphereDensity * (k1 + 4*k2 + k3);

		f *= (1 / exp(stepLength * density));

		currentPosition += (stepLength * view);
	}

	return f;
}

float atmosphereSurface(float3 fragmentPosition, float3 viewPosition, float3 lightPosition, float atmosphereRadius, float planetRadius, float scaleHeight, float atmosphereDensity, int samplePoints)
{
	float3 view = normalize(viewPosition - fragmentPosition);

	// Find the "real" point on the sphere
	//float3 p = fragmentPosition + raySphereIntersectDistance(fragmentPosition, view, planetRadius).y * view;
	float3 p = fragmentPosition;

	// recalculate view
	view = normalize(viewPosition - p);

	float depth = min(raySphereIntersectDistance(p, view, atmosphereRadius).y, length(viewPosition - p));
	float f = 1.0;
	float stepLength = depth / samplePoints;
	float halfStepLength = stepLength / 2.0;
	float3 currentPosition = p;

	for (int i = 0; i < samplePoints; i++)
	{
		float3 light = normalize(lightPosition - currentPosition);

		float height = max((length(currentPosition) - planetRadius) / (atmosphereRadius - planetRadius), 0.0);
		float k1 = exp(-height / scaleHeight);
		height = max((length(currentPosition + halfStepLength * view) - planetRadius) / (atmosphereRadius - planetRadius), 0.0);
		float k2 = exp(-height / scaleHeight);
		height = max((length(currentPosition + stepLength * view) - planetRadius) / (atmosphereRadius - planetRadius), 0.0);
		float k3 = exp(-height / scaleHeight);

		float density = atmosphereDensity * (k1 + 4*k2 + k3);

		f *= (1 / exp(stepLength * density));

		currentPosition += (stepLength * view);
	}

	return f;
}

/*
float noise(float3 p, sampler1D rt)
{
		// Compute the integer positions of the four surrounding points
		int qx0 = (int) p.x;
		int qx1 = qx0 + 1;
		int qy0 = (int) p.y;
		int qy1 = qy0 + 1;
		int qz0 = (int) p.z;
		int qz1 = qz0 + 1;
		// Permutate values to get indices to use with the gradient look-up tables
		float q000 = tex1D(rt, qz0 + tex1D(rt, qy0/256.0 + tex1D(rt, qx0/256.0).a).a).a;
		float q001 = tex1D(rt, qz0 + tex1D(rt, qy0/256.0 + tex1D(rt, qx1/256.0).a).a).a;
		float q010 = tex1D(rt, qz0 + tex1D(rt, qy1/256.0 + tex1D(rt, qx0/256.0).a).a).a;
		float q011 = tex1D(rt, qz0 + tex1D(rt, qy1/256.0 + tex1D(rt, qx1/256.0).a).a).a;
		float q100 = tex1D(rt, qz1 + tex1D(rt, qy0/256.0 + tex1D(rt, qx0/256.0).a).a).a;
		float q101 = tex1D(rt, qz1 + tex1D(rt, qy0/256.0 + tex1D(rt, qx1/256.0).a).a).a;
		float q110 = tex1D(rt, qz1 + tex1D(rt, qy1/256.0 + tex1D(rt, qx0/256.0).a).a).a;
		float q111 = tex1D(rt, qz1 + tex1D(rt, qy1/256.0 + tex1D(rt, qx1/256.0).a).a).a;

		// Computing vectors from the four points to the input point
		float tx0 = p.x - qx0;
		float tx1 = tx0 - 1;
		float ty0 = p.y - qy0;
		float ty1 = ty0 - 1;
		float tz0 = p.z - qz0;
		float tz1 = tz0 - 1;
		// Compute the dot-product between the vectors and the gradients
		float v000 = dot(2*tex1D(rt, q000).xyz-1.0, float3(tx0, ty0, tz0));
		float v001 = dot(2*tex1D(rt, q001).xyz-1.0, float3(tx1, ty0, tz0));
		float v010 = dot(2*tex1D(rt, q010).xyz-1.0, float3(tx0, ty1, tz0));
		float v011 = dot(2*tex1D(rt, q011).xyz-1.0, float3(tx1, ty1, tz0));
		float v100 = dot(2*tex1D(rt, q100).xyz-1.0, float3(tx0, ty0, tz1));
		float v101 = dot(2*tex1D(rt, q101).xyz-1.0, float3(tx1, ty0, tz1));
		float v110 = dot(2*tex1D(rt, q110).xyz-1.0, float3(tx0, ty1, tz1));
		float v111 = dot(2*tex1D(rt, q111).xyz-1.0, float3(tx1, ty1, tz1));

		// Do the bi-cubic interpolation to get the final value
		float wx = (3 - 2*tx0)*tx0*tx0;
		float v00 = v000 - wx*(v000 - v001);
		float v01 = v010 - wx*(v010 - v011);
		float v10 = v100 - wx*(v100 - v101);
		float v11 = v110 - wx*(v110 - v111);
		float wy = (3 - 2*ty0)*ty0*ty0;
		float v0 = v00 - wy*(v00 - v01);
		float v1 = v10 - wy*(v10 - v11);
		float wz = (3 - 2*tz0)*tz0*tz0;
		float v = v0 - wz*(v0 - v1);

		return v;
}

float beta_ab()
{
	return 0.0;
}

float beta_sc()
{
	return 0;
}

float beta_ex()
{
	return beta_ab() + beta_sc();
}

float extinction(float lambda, float3 rayStart, float3 rayEnd)
{
}

float inscattering()
{
}

const float red = 650.0;
const float green = 510.0;
const float blue = 475.0;

float3 atmosphere2(float3 lightColor, float3 fragmentPosition, float3 viewPosition, float3 lightPosition, float atmosphereRadius, float planetRadius)
{
	float3 rayDir = normalize(fragmentPosition - viewPosition);
	float3 rayEnd = viewPosition + rayDir * raySphereIntersectDistance(viewPosition, rayDir, atmosphereRadius).z;
	
	float ex = extinction();
}
*/

static const float3 betaR = { 6.95e-6, 1.18e-5, 2.44e-5 };
static const float3 betaM = { 4e-7, 6e-7, 2.4e-6 };
//static const float3 betaR = { 2.44e-5, 1.18e-5, 6.95e-6 };
//static const float3 betaM = { 2.4e-6, 6e-7, 4e-7 };
static const float3 Esun = { 8.0, 8.0, 8.0 };
static const float g = -0.375;
static const float exposure = 1.1;

float3 f_ex(float s) {
	return exp(-(betaR + betaM) * s);
}

float3 beta_r(float cosTheta) {
	return (3.0 / (16.0 * PI)) * betaR * (1 + cosTheta*cosTheta);
}

float3 beta_m(float cosTheta) {
	float g2 = g*g;
	float x1 = (1-g)*(1-g);
	float x2 = 2*g*cosTheta;
	float x3 = 1 + g2 - x2;

	return (1.0 / (4.0 * PI)) * (x1 / pow(x3, 1.5)) * betaM;
}

float3 l_in(float s, float cosTheta) {
	return ((beta_r(cosTheta) + beta_m(cosTheta)) / (betaR + betaM)) * Esun * (1 - f_ex(s));
}

float3 atmosphere2(float3 color, float3 fragPos, float3 viewPos, float3 directionToLight, float planetRadius, float atmosphereRadius)
{
	float3 viewDir = normalize(fragPos - viewPos);
	float cosTheta = dot(viewDir, -directionToLight);

	float atmosphereNearIntersection = raySphereIntersectDistance(viewPos, viewDir, atmosphereRadius).z;
	if (atmosphereNearIntersection > 0)
	{
		viewPos += (atmosphereNearIntersection * viewDir);
	}

	float viewDistance = distance(fragPos, viewPos);

	//float sPart = viewDistance / 100.0;

	/*
	for (int i = 0; i < 100; i++)
	{
		float3 samplePos = fragPos - (i*sPart) * viewDir;
		color *= f_ex(1000.0 * sPart);
		if (!raySphereIntersect(samplePos, directionToLight, planetRadius))
		{
			// Sun is not obstructed by planet, add more light
			color += l_in(1000.0 * sPart, cosTheta);
		}
	}
	*/

	color *= f_ex(1000.0 * viewDistance);
	color += l_in(1000.0 * viewDistance, cosTheta);
	return 1.0 - exp(-exposure*color);
}