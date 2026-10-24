#version 330

in	vec4			vertexColor;
in	vec2			texCoordinate;

out	vec4			color;


uniform sampler2D	textureObj;



void	main()
{
	// color = vertexColor;
	color = texture(textureObj, texCoordinate);

}
