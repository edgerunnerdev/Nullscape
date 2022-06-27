#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
}

#elif defined FRAGMENT_PROGRAM

out vec4 colour;

void main()
{
	colour = vec4( 0, 1, 1, 1 );
}

#endif