#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

out Vertex
{
	vec2 UV;
	vec3 position;
	mat3 tangentBasis;
	vec3 viewDir;
} vout;

uniform mat4 k_worldViewProj;
uniform mat4 k_world;
uniform mat4 k_worldInverseTranspose;
uniform mat4 k_viewInverse;

void main()
{
	gl_Position = k_worldViewProj * vec4( position, 1 );
	vout.UV = UV;
	vout.position = vec4( k_world * vec4( position, 1 ) ).xyz;
	vout.tangentBasis = mat3(k_world) * mat3(bitangent, tangent, normal);
	vec4 po = vec4( gl_Position.xyz , 1 );
	vec3 pw = vec4( k_worldInverseTranspose * po ).xyz;
	vout.viewDir = normalize( vec3( k_viewInverse[0].w, k_viewInverse[1].w, k_viewInverse[2].w ) - pw );
}
