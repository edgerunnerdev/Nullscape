#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
// layout(location = 2) is vec3 vertexNormal;
layout(location = 3) in vec4 vertexColor;

out vec2 UV;
out vec4 vcolor;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
	UV = vertexUV;
	vcolor = vertexColor;
}

#elif defined FRAGMENT_PROGRAM

in vec2 UV;
in vec4 vcolor;

out vec4 color;

uniform float k_time = 0.0;
uniform sampler2D k_sampler0;

float rand( vec2 co ) 
{
	return fract( sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453 );
}

const float kStarSize = 0.15;
const float kStarBrightness = 0.5; // Value >= 0.0 and < 4.0
const vec3 kStarColor = vec3(1.0, 0.7, 0.0);

float getGlow(vec2 uv)
{
    // Gentle slope for the glow:
    // https://www.desmos.com/calculator/fjwv1ctp4h
    float pi = 3.14159;
    float x = min(length(uv), 1.);
    float l = cos(x*pi) / 2. + 0.5; 
    l = (l + (1.-x))/2.;
    return l;
}

float getIntensity(vec2 uv)
{
    float x = min(length(uv), 1.);   
    return pow(1. - (x - kStarSize), 4. - kStarBrightness);
}

void main()
{
	float r = rand( vec2( UV.x + k_time, UV.y + k_time ) );
	color = vcolor * texture( k_sampler0, UV );
	color = color * 0.9 + color * r * 0.1;

    // Center the UVs in the middle of the surface so we are working
    // in -1 to 1, with 0 being the center of the star.
    vec2 centerUV = 2. * (UV - vec2(0.5));

    vec3 glowColor = vcolor.rgb * getGlow(centerUV); 
    vec3 intensityColor = vec3(1.) * getIntensity(centerUV);
    vec3 col = mix(glowColor, intensityColor, intensityColor.r );

    // Output to screen
    color = color * 0.0001 + vec4(col * 0.8 + col * r * 0.2, intensityColor.r);

}

#endif