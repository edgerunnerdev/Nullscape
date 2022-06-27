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

uniform float k_time = 0.0;
uniform sampler2D k_sampler0;

float rand( vec2 co ) 
{
	return fract( sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453 );
}

void main()
{
	float r = rand( vec2( UV.x + k_time, UV.y + k_time ) );
	colour = vcolour * texture( k_sampler0, UV );
	colour = colour * 0.4 + colour * r * 0.6;
}

#endif