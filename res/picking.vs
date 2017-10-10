#version 440

in vec2 texcoord2D;
in vec3 texcoordCube;
in vec3 position;
in vec3 normal;

uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	vec4 realPos = vec4(position.xyz, 1.0);
	gl_Position = vec4(modelViewMatrix * projMatrix * realPos);
}
