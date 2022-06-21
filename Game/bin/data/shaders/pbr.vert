#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out Vertex
{
	vec2 UV;
	vec3 objPosition;
	vec3 normal;
	vec3 viewDir;
} vout;

uniform mat4 k_worldViewProj;
uniform mat4 k_world;
uniform mat4 k_worldInverseTranspose;
uniform mat4 k_viewInverse;

void main()
{
	gl_Position = k_worldViewProj * vec4( vertexPosition, 1 );
	vout.UV = vertexUV;
	vout.objPosition = vec4( k_world * vec4( vertexPosition, 1 ) ).xyz;
	vout.normal = vec4( k_worldInverseTranspose * vec4( normalize( vertexNormal ), 1 ) ).xyz;
	vec4 po = vec4( gl_Position.xyz , 1 );
	vec3 pw = vec4( k_worldInverseTranspose * po ).xyz;
	vout.viewDir = normalize( vec3( k_viewInverse[0].w, k_viewInverse[1].w, k_viewInverse[2].w ) - pw );
}
