#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
	UV = vertexUV;
}

#elif defined FRAGMENT_PROGRAM

in vec2 UV;

out vec4 colour;

uniform sampler2D k_sampler0;
uniform vec4 k_colour = vec4( 1.0, 1.0, 1.0, 1.0 );

void main()
{
	colour = texture( k_sampler0, UV ) * k_colour;
}

#endif