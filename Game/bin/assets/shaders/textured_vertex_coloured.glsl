#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
// layout(location = 2) is vec3 vertexNormal;
layout(location = 3) in vec4 vertexColour;

out vec2 UV;
out vec4 vcolour;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
	UV = vertexUV;
	vcolour = vertexColour;
}

#elif defined FRAGMENT_PROGRAM

in vec2 UV;
in vec4 vcolour;

out vec4 colour;

uniform sampler2D k_sampler0;

void main()
{
	colour = vcolour * texture( k_sampler0, UV );
}

#endif