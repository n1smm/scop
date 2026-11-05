#version 330

in	vec4			vertexColor;
in	vec2			texCoordinate;
in	vec3			bary;
flat in	int			surfaceSwitchPass;
flat in float		runTime;

out	vec4			color;


uniform sampler2D	textureObj;



void	main()
{
	// float delay = gl_FragCoord.x / 800.0;
	// if (runTime < delay)
	// 	discard;
	float delay = runTime * 1;

	if (surfaceSwitchPass == 0)
	{
		float nearestEdge = min(min(bary.x, bary.y), bary.z);
		if (nearestEdge < 0.03)
			color = vec4(0.0, 0.0, 0.0, 1.0);
		else
			color = vertexColor;
	}
	else if (surfaceSwitchPass == 2)
		color = vertexColor;
	else
		color = texture(textureObj, texCoordinate);
	if (runTime < 0.0)
		color = vec4(0.0);

}
