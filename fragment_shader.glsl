#version 150

precision highp float;

in vec3 _Color;

void main()
{
	gl_FragColor = vec4(_Color, 1.0);
}
