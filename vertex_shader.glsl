#version 330

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec3 in_Color;

out vec3 _Color;

void main()
{
	gl_Position = vec4(in_Position, 0.0, 1.0);

	_Color = in_Color;
}
