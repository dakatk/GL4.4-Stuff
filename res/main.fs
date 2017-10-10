#version 440
#extension GL_NV_shadow_samplers_cube : enable

in vec2 Texcoord2D;
in vec3 TexcoordCube;
in vec3 Normal;
in vec3 WorldPos;

out vec4 gl_FragColor;

struct Material {
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 pos;
};

const int max_lights = 10;

uniform int num_lights;
uniform int is_skybox;
uniform int picked;

uniform struct Light lights[max_lights];
uniform struct Material material;

uniform sampler2D tex2D;
uniform samplerCube texCube;

void main()
{
	vec3 finalColor = vec3(0.0);
	vec3 norm = normalize(Normal);
	vec4 objectColor = texture2D(tex2D, Texcoord2D);
	vec4 skyboxColor = textureCube(texCube, TexcoordCube);

	for(int i = 0; i < num_lights && i < max_lights; i ++)
	{
		struct Light light = lights[i];

		vec3 ambient = light.ambient * material.ambient;
		vec3 lightDir = normalize(light.pos - WorldPos);

		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse);

		vec3 viewDir = normalize(-WorldPos);
		vec3 reflectDir = reflect(-lightDir, norm); 

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * material.specular); 

		finalColor = finalColor + (objectColor.rgb * (ambient + diffuse + specular));
	}
	if(picked == 1)
	{
		finalColor = mix(finalColor, vec3(1.0, 0.0, 0.0), 0.75);
	}

	vec4 t0 = vec4(finalColor, objectColor.a);
	vec4 t1 = vec4(skyboxColor.rgb, 1.0);

	gl_FragColor = mix(t0, t1, is_skybox);
}

