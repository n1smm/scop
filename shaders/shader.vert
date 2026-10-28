#version 330
layout	(location = 0) in vec3	pos;
layout	(location = 1) in vec2	tex;
layout	(location = 2) in vec3	norm;

uniform	mat4	model;
uniform	mat4	projection;
uniform	mat4	view;
uniform	vec3	inColor;

uniform int		surfaceSwitch;


out		vec4	vertexColor;	
out		vec2	texCoordinate;
out		vec3	bary;
flat out int	surfaceSwitchPass;

void	main()
{
	gl_Position = projection * view * model * vec4(pos, 1);
	if (dot(inColor, inColor) != 0.0)
		vertexColor = vec4(inColor, 1.0f);
	else
		vertexColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

	int corner = gl_VertexID % 3;
	bary = vec3(corner == 0, corner == 1, corner == 2);

	texCoordinate = tex;
	surfaceSwitchPass = surfaceSwitch;
}
