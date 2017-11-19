#version 330

precision highp float;

in vec3 _Color;
out vec4 outColor;

void main()
{
	outColor = vec4(_Color, 1.0);
}
