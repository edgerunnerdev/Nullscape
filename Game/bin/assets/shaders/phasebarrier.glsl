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
uniform float k_time = 0;
uniform float k_shieldStrength = 1;	// Value between 0 and 1

float rand( vec2 co ) 
{
	return fract( sin( dot( co.xy , vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

void main()
{
	// Shield noise becomes stronger the weaker it is
	// Maximum shield strength is capped at 0.9 so we always get some noise getting through
	float ns = rand( vec2( UV.x + k_time, UV.y + k_time ) ) / 2 + 0.5;
	float shieldNoiseMultiplier = 1 - ns * ( 1 - min( k_shieldStrength, 0.9 ) ); 

	vec2 uv = vec2( UV.x, UV.y + sin( k_time ) * 0.1 );
	vec4 grid = texture( k_sampler0, uv ) + texture( k_sampler0, UV );

	colour = clamp( vcolour * shieldNoiseMultiplier + vec4( 0.2, 0.2, 0.2, 0 ), 0, 1 ) * grid;
}

#endif