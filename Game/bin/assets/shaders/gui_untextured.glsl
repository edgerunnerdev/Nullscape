#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
}

#elif defined FRAGMENT_PROGRAM

out vec4 colour;

uniform vec4 k_colour = vec4( 1, 0, 0, 0.5 );

void main()
{
	colour = k_colour;
}

#endif