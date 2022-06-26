#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 objPosition;
out vec3 normal;
out vec3 viewDir;

uniform mat4 k_worldViewProj;
uniform mat4 k_world;
uniform mat4 k_worldInverseTranspose;
uniform mat4 k_viewInverse;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
	objPosition = vec4( k_world * gl_Position ).xyz;
	UV = vertexUV;
	normal = vec4( k_worldInverseTranspose * vec4( normalize( vertexNormal ), 1 ) ).xyz;
	vec4 po = vec4( gl_Position.xyz , 1 );
	vec3 pw = vec4( k_worldInverseTranspose * po ).xyz;
	viewDir = normalize( vec3( k_viewInverse[0].w, k_viewInverse[1].w, k_viewInverse[2].w ) - pw );
}

#elif defined FRAGMENT_PROGRAM

in vec2 UV;
in vec3 objPosition;
in vec3 normal;
in vec3 viewDir;

out vec4 colour;

uniform sampler2D k_sampler0;	// energy texture
uniform float k_health = 1;
uniform float k_time = 0;
uniform int k_empActive = 0;

void main()
{
	//float4 objPosWorld = float4(objPosition - k_clip.xyz, 1.0);
	//if ( dot(normalize(objPosWorld), k_clipForward) > 0.0f )
	//	discard;

	if ( k_health <= 0 || k_empActive == 1 )
	{
		colour = vec4( 0, 0, 0, 1 );
	}
	else
	{
		vec2 uv  = -UV * 1.5 + vec2( 0.1 * k_time, 1.2 * k_time );
		vec2 uv2 = (UV + vec2( 0.5 * cos( k_time ), 0.3 * k_time ) );
		vec4 diffuse =  texture( k_sampler0, uv );
		vec4 diffuse2 = texture( k_sampler0, uv2 );

		colour = clamp(diffuse - diffuse2, 0, 1) * vec4( 0.31, 0.85, 0.85, 1 );
	}
}

#endif