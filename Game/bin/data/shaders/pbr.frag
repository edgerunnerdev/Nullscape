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

uniform vec3 LightPosition[3];
uniform vec3 LightColor[3];

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
	vec3 N = normalize(2.0 * texture(NormalSampler, vin.UV).rgb - 1.0);
	N = normalize(vin.tangentBasis * N);
	//N = N * 0.00000001 + normalize(vin.tangentBasis * vec3(1));

	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// Direct lighting calculation for analytical lights.
	int NumLights = 3;
	vec3 directLighting = vec3(0);
	for(int i=0; i<NumLights; ++i)
	{
		vec3 lightDir = normalize(LightPosition[i] - vin.position);
		vec3 Li = lightDir;
		vec3 Lradiance = LightColor[i];

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

	vec3 ambientLighting;
	{
		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = vec3(0.1, 0.3, 0.6); //texture(irradianceTexture, N).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		vec3 F = fresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = kd * albedo * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		//int specularTextureLevels = textureQueryLevels(specularTexture);
		//vec3 specularIrradiance = textureLod(specularTexture, Lr, roughness * specularTextureLevels).rgb;
		vec3 specularIrradiance = vec3(0.5 * roughness); // HACK!

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		//vec2 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;
		vec2 specularBRDF = 0.5 * vec2(cosLo, roughness); // HACK

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
		ambientLighting = diffuseIBL + specularIBL;
	}

	directLighting += texture(HeightSampler, vin.UV).rgb * 0.00001;

	color = vec4(directLighting + ambientLighting, 1);
}
