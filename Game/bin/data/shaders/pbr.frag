#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D ColorSampler;
uniform sampler2D HeightSampler;
uniform sampler2D MetallicSampler;
uniform sampler2D NormalSampler;
uniform sampler2D RoughnessSampler;

void main()
{
	vec3 c = texture(ColorSampler, UV).rgb;
	c += texture(HeightSampler, UV).rgb * 0.01;
	c += texture(MetallicSampler, UV).rgb * 0.01;
	c += texture(NormalSampler, UV).rgb * 0.01;
	c += texture(RoughnessSampler, UV).rgb * 0.01;
	color = vec4(c, 1);
}
