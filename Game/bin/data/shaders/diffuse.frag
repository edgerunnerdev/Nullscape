#version 330 core

in vec2 UV;

out vec4 colour;

uniform sampler2D k_sampler0;

void main()
{
	colour = vec4(texture( k_sampler0, UV ).rgb, 1);
}
