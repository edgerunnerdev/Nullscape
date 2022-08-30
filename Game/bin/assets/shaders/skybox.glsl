#if defined VERTEX_PROGRAM

layout(location = 0) in vec3 vertexPosition;

out vec3 UVW;

uniform mat4 k_worldViewProj;

void main()
{
	gl_Position = k_worldViewProj * vec4(vertexPosition, 1);
	UVW = vertexPosition;
}

#elif defined FRAGMENT_PROGRAM

in vec3 UVW;

out vec4 color;

uniform samplerCube k_cubemap;

void main()
{
	color = vec4(texture(k_cubemap, UVW).rgb, 1);
}

#endif