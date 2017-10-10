#version 440

in vec2 texcoord2D;
in vec3 texcoordCube;
in vec3 position;
in vec3 normal;

out vec2 Texcoord2D;
out vec3 TexcoordCube;
out vec3 Normal;
out vec3 WorldPos;

uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	Texcoord2D = texcoord2D;
	TexcoordCube = texcoordCube;
	
	vec4 realPos = vec4(position.xyz, 1.0);
	mat4 modelMatrix = inverse(viewMatrix) * modelViewMatrix;
	
	Normal = mat3(transpose(inverse(modelMatrix))) * normal;
	WorldPos = (modelMatrix * realPos).xyz;
	
	gl_Position = vec4(modelViewMatrix * projMatrix * realPos);
}
