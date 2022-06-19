#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D ColorSampler;
uniform sampler2D HeightSampler;
uniform sampler2D MetallicSampler;
uniform sampler2D NormalSampler;
uniform sampler2D RoughnessSampler;

void main()
{
	color = texture(ColorSampler, UV).rgb;
	color += texture(HeightSampler, UV).rgb * 0.01;
	color += texture(MetallicSampler, UV).rgb * 0.01;
	color += texture(NormalSampler, UV).rgb * 0.01;
	color += texture(RoughnessSampler, UV).rgb * 0.01;
}
