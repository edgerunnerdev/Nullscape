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

out vec4 color;

uniform sampler2D k_sampler0;
uniform sampler2D k_sampler1;
uniform vec2 k_resolution;

uniform bool k_applyBleachBypass = true;
uniform bool k_applyGlow = true;
uniform bool k_applyVignette = true;

vec4 ApplyBleachBypass( vec4 c )
{
	vec3 base = c.rgb;
	vec3 lumCoeff = vec3( 0.25, 0.65, 0.1 );
	float lum = dot( lumCoeff, base );
	vec3 blend = vec3( lum );
	float L = min( 1, max( 0, 10 * ( lum - 0.45 ) ) );
	vec3 result1 = 2 * base * blend;
	vec3 result2 = 1 - 2 * ( 1 - blend ) * ( 1 - base );
	return vec4( mix( result1, result2, L ), 1 );
}

vec4 ApplyGlow( vec4 c )
{
	vec4 glow = texture( k_sampler1, UV );
	return min( c + glow, 1 );
}

void main()
{
	color = vec4( texture( k_sampler0, UV ).rgb, 1 );

	if ( k_applyGlow )
	{
		color = ApplyGlow( color );
	}

	if ( k_applyVignette )
	{
		color *= 0.3 + 0.7 * pow( 24.0 * UV.x * UV.y * ( 1.0 - UV.x ) * ( 1.0 - UV.y ), 0.12 );
	}
}

#endif