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

void main()
{
	color = texture( k_sampler0, UV );

	vec2 dividerLineP1 = vec2( 0.25, 0.0 );
	vec2 dividerLineP2 = vec2( 0.75, 1.0 );
	vec2 testP3 = UV;

	float det = ( dividerLineP2.x - dividerLineP1.x ) * ( testP3.y - dividerLineP1.y ) - ( dividerLineP2.y - dividerLineP1.y ) * ( testP3.x - dividerLineP1.x );
	if ( det > 0 )
	{
		color.a = 0.0;
	}
	else
	{
		color.a = 1.0;
	}

	if ( 1.0 - min( length( det ) * 290.0f, 1.0 ) > 0.0 )
	{
		color = vec4( 0.9, 0.9, 0.9, 0.2 );
	}

	if ( 1.0 - min( length( det ) * 300.0f, 1.0 ) > 0.0 )
	{
		color = vec4( 0.9, 0.9, 0.9, 1.0 );
	}
}

#endif