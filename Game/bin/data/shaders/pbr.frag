#version 330 core

in Vertex
{
	vec2 UV;
	vec3 objPosition;
	vec3 normal;
	vec3 viewDir;
} vin;

out vec4 color;

uniform sampler2D ColorSampler;
uniform sampler2D HeightSampler;
uniform sampler2D MetallicSampler;
uniform sampler2D NormalSampler;
uniform sampler2D RoughnessSampler;

void main()
{
	vec3 albedo = texture(ColorSampler, vin.UV).rgb;
	float metalness = texture(MetallicSampler, vin.UV).r;
	float roughness = texture(RoughnessSampler, vin.UV).r;

	vec3 c = texture(ColorSampler, vin.UV).rgb;
	c += texture(HeightSampler, vin.UV).rgb * 0.01;
	c += texture(MetallicSampler, vin.UV).rgb * 0.01;
	c += texture(NormalSampler, vin.UV).rgb * 0.01;
	c += texture(RoughnessSampler, vin.UV).rgb * 0.01;
	color = vec4(c, 1);
}
