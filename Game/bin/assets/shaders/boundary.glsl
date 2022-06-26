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

uniform float k_time = 0;

float rand( vec2 co ) 
{
	return fract( sin( dot( co.xy , vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

void main()
{
	float ns = rand( vec2( UV.x + k_time, UV.y + k_time ) ) / 2 + 0.1;
	colour = vec4( ns, 0.0, 0.0, 1.0 );
}

#endif