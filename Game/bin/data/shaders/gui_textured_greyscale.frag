#version 330 core

in vec2 UV;

out vec4 colour;

uniform sampler2D k_sampler0;

void main()
{
	colour = texture( k_sampler0, UV );
	float g = sqrt( dot( colour * colour, vec4( 0.299, 0.587, 0.114, 0.000 ) ) );
	g = min( g, 0.8 );
	colour = vec4( g );
}
