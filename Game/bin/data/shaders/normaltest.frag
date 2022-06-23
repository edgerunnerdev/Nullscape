#version 330 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04); // Constant normal incidence Fresnel factor for all dielectrics.

in Vertex
{
	vec2 UV;
	vec3 position;
	mat3 tangentBasis;
	vec3 viewDir;
} vin;

out vec4 color;

uniform mat4 k_viewInverse;

uniform sampler2D ColorSampler;
uniform sampler2D HeightSampler;
uniform sampler2D MetallicSampler;
uniform sampler2D NormalSampler;
uniform sampler2D RoughnessSampler;

uniform vec3 LightPosition;
uniform vec3 LightColor;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = texture(ColorSampler, vin.UV).rgb;
	float metalness = texture(MetallicSampler, vin.UV).r;
	float roughness = texture(RoughnessSampler, vin.UV).r;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 eyePosition = vec3(k_viewInverse[3]);
	vec3 Lo = normalize(eyePosition - vin.position);

	// Get current fragment's normal and transform to world space.
	vec3 N = texture(NormalSampler, vin.UV).rgb;
	N = normalize(2.0 * N - 1.0);
	N = normalize(vin.tangentBasis * N);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);

	vec3 lightDir = normalize(LightPosition - vin.position);
	vec3 Li = lightDir;

	// Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(N, Li));
	
	directLighting += albedo * cosLi;

	directLighting += (texture(HeightSampler, vin.UV).rgb + albedo + metalness + roughness + texture(NormalSampler, vin.UV).rgb) * 0.00001;

	color = vec4(directLighting, 1);
}
