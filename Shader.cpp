#include "Shader.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <fstream>

#include <cstdio>

// returns the length of a given file
static unsigned int getFileLength(ifstream& file)
{
    if(!file.good()) 
		return 0;
    
    file.tellg();
    file.seekg(0, ios::end);

    unsigned int len = file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

// loads the shaders source to a char array,
// returns false if there was an error while
// reading the file
static bool loadShaderSource(const char* filename, char** shaderSource, int* len)
{
   	ifstream file;
   	file.open(filename, ios::in);

  	if(!file) 
		return false;
    
   	*len = (int)getFileLength(file);
   	if(*len == 0) 
		return false;
    
   	*shaderSource = new char[(*len) + 1];
   	if(*shaderSource == 0)
		return false;
   
   	(*shaderSource)[*len] = 0; 

   	unsigned int i = 0;
   	while (file.good())
   	{
		(*shaderSource)[i] = file.get();
       	if(!file.eof())
        	i ++;
   	}

   	(*shaderSource)[i] = 0;
  	file.close();
      
   	return true;
}

// loads a vertex shader file and a fragment
// shader file into a single shader program
// wrapped in a Shader class object. Errors
// are checked for and assessed wherever necessary
// during the loading process, to ensure the
// resulting shader program will run without issues
Shader::Shader(string vertfile, string fragfile)
{
	unsigned int vert_shader, frag_shader;

	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	char* vertSource;
	char* fragSource;

	int flength, vlength;

	if(!loadShaderSource(vertfile.c_str(), &vertSource, &vlength))
		cout << "Failed to load vertex shader!" << endl;

	if(!loadShaderSource(fragfile.c_str(), &fragSource, &flength))
		cout << "Failed to load fragment shader!" << endl;

	glShaderSource(vert_shader, 1, &vertSource, &vlength);
	glShaderSource(frag_shader, 1, &fragSource, &flength);

	delete[] vertSource;
	delete[] fragSource;

	glCompileShader(vert_shader);
	glCompileShader(frag_shader);

	int compiled, v_comp, f_comp;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &v_comp);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &f_comp);
	
	compiled = (v_comp && f_comp ? GL_TRUE : GL_FALSE);
	if(compiled == GL_FALSE)
	{
		int blen = 0;	
		GLsizei slen = 0;
		
		if(v_comp == GL_FALSE)
		{
			glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &blen);       
			if(blen > 1)
			{
 				char* compiler_log = (char*)malloc(blen);
 				glGetShaderInfoLog(vert_shader, blen, &slen, compiler_log);

 				cout << "Vertex shader error:\n" << compiler_log << endl;
 				free(compiler_log);

				exit(1);
			}
		}
		if(!f_comp == GL_FALSE)
		{
			glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &blen);       
			if(blen > 1)
			{
 				char* compiler_log = (char*)malloc(blen);
 				glGetShaderInfoLog(frag_shader, blen, &slen, compiler_log);

 				cout << "Fragment shader error:\n" << compiler_log << endl;
 				free(compiler_log);

				exit(1);
			}
		}
	}
	this->prog_id = glCreateProgram();

	glAttachShader(this->prog_id, vert_shader);
	glAttachShader(this->prog_id, frag_shader);

	glBindFragDataLocation(this->prog_id, 0, FRAG_COLOR_STR);
	glBindAttribLocation(this->prog_id, NORMAL_ATTR, NORMAL_STR);
	glBindAttribLocation(this->prog_id, POSITION_ATTR, POSITION_STR);
    glBindAttribLocation(this->prog_id, TEXCOORD_2D_ATTR, TEXCOORD_2D_STR);
	glBindAttribLocation(this->prog_id, TEXCOORD_CUBE_ATTR, TEXCOORD_CUBE_STR);

	glLinkProgram(this->prog_id);

	int linked;
	glGetProgramiv(this->prog_id, GL_LINK_STATUS, &linked);

	if(linked == GL_FALSE)
	{
		int blen = 0;	
		GLsizei slen = 0;

		glGetProgramiv(this->prog_id, GL_INFO_LOG_LENGTH, &blen);       
		if(blen > 1)
		{
 			char* compiler_log = (char*)malloc(blen);
 			glGetProgramInfoLog(this->prog_id, blen, &slen, compiler_log);

 			cout << "Program linking error:\n" << compiler_log << endl;
 			free(compiler_log);

			exit(1);
		}
	}
	glDetachShader(this->prog_id, vert_shader);
	glDetachShader(this->prog_id, frag_shader);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	this->begin();

	this->cm_mat_loc = glGetUniformLocation(this->prog_id, CAMERA_MATRIX_STR);
	this->mv_mat_loc = glGetUniformLocation(this->prog_id, MODELVIEW_MATRIX_STR);
	this->pr_mat_loc = glGetUniformLocation(this->prog_id, PROJECTION_MATRIX_STR);
	
	int tex_cube_loc = glGetUniformLocation(this->prog_id, TEXTURE_CUBE_STR);
	int tex_2d_loc = glGetUniformLocation(this->prog_id, TEXTURE_2D_STR);

	glUniform1i(tex_cube_loc, TEXTURE_CUBE_ID);
	glUniform1i(tex_2d_loc, TEXTURE_2D_ID);

	this->num_lights = 0;
	this->setUniformi(NUM_LIGHTS_STR, this->num_lights);

	this->end();
}

// deletes the loaded shader program for this Shader class object
Shader::~Shader()
{
	glDeleteProgram(this->prog_id);
}

// binds the shader program to be used for rendering
void Shader::begin()
{
	glUseProgram(this->prog_id);
}

// sets the current texture index to be used
// when texture ids are pushed to the shader
// (should just be TEXTURE0 in most case, but
// needs to be allowed for multiple offsets for
// models with multiple textures)
void Shader::setTexture(int num)
{
	glActiveTexture(GL_TEXTURE0 + (unsigned int)num);
}

// pushes an integer value to the corresponding
// uniform variable in the active shader set
void Shader::setUniformi(string name, int value)
{
	int var_loc = glGetUniformLocation(this->prog_id, name.c_str());
	glUniform1i(var_loc, value);
}

// recursively sets light values in the active
// shader set based on values from a list of
// Light class objects
void Shader::setLighting(Light** lights, int amount)
{
	int i;
	for(i = 0; i < amount; i ++)
		this->setLight(lights[i], i);
}

// same as 'setLighting', but sets only from a
// single Light object
void Shader::setLight(Light* light, int n)
{
	char* l_specular =  new char[50];
	char* l_ambient = new char[50];
	char* l_diffuse = new char[50];
	char* l_pos = new char[50];

	if(this->num_lights <= n)
	{
		this->num_lights = n + 1;
		this->setUniformi(NUM_LIGHTS_STR, this->num_lights);
	}

	snprintf(l_specular, 50, "lights[%d].specular", n);
	snprintf(l_ambient, 50, "lights[%d].ambient", n);
	snprintf(l_diffuse, 50, "lights[%d].diffuse", n);
	snprintf(l_pos, 50, "lights[%d].pos", n);

	int l_specular_loc = glGetUniformLocation(this->prog_id, l_specular);
	int l_ambient_loc = glGetUniformLocation(this->prog_id, l_ambient);
	int l_diffuse_loc = glGetUniformLocation(this->prog_id, l_diffuse);
	int l_pos_loc = glGetUniformLocation(this->prog_id, l_pos);

	glUniform3fv(l_specular_loc, 1, light->getSpecular());
	glUniform3fv(l_ambient_loc, 1, light->getAmbient());
	glUniform3fv(l_diffuse_loc, 1, light->getDiffuse());
	glUniform3fv(l_pos_loc, 1, light->getPos());

	delete[] l_specular;
	delete[] l_ambient;
	delete[] l_diffuse;
	delete[] l_pos;
}

// sets values for the active shader set based
// on the values from a Material class object
void Shader::setMaterial(Material* material)
{
	string m_ambient = "material.ambient";
	string m_diffuse = "material.diffuse";
	string m_specular = "material.specular";
	string m_shininess = "material.shininess";

	int m_ambient_loc = glGetUniformLocation(this->prog_id, m_ambient.c_str());
	int m_diffuse_loc = glGetUniformLocation(this->prog_id, m_diffuse.c_str());
	int m_specular_loc = glGetUniformLocation(this->prog_id, m_specular.c_str());
	int m_shininess_loc = glGetUniformLocation(this->prog_id, m_shininess.c_str());

	glUniform3fv(m_ambient_loc, 1, material->getAmbient());
	glUniform3fv(m_diffuse_loc, 1, material->getDiffuse());
	glUniform3fv(m_specular_loc, 1, material->getSpecular());
	glUniform1f(m_shininess_loc, material->getShininess());
}

// sets the view matrix to the active
// shader program, to be used after world
// or camera translations but before object
// translations
void Shader::setCameraMatrix()
{
	float* cm_mat = new float[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, cm_mat);
	glUniformMatrix4fv(this->cm_mat_loc, 1, GL_FALSE, cm_mat);

	delete[] cm_mat;
}

// sets the modelview matrix to the active
// shader program, to be used after translations
// for the camera and current world object
// have been done
void Shader::setModelViewMatrix()
{
	float* mv_mat = new float[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, mv_mat);
	glUniformMatrix4fv(this->mv_mat_loc, 1, GL_FALSE, mv_mat);

	delete[] mv_mat;
}

// sets the projection matrix to the shader program,
// to be used before anything happens in the modelview
// matrix mode of the render loop. Describes the window's
// viewport and 2D perspective
void Shader::setProjectionMatrix()
{
	float* pr_mat = new float[16];

	glGetFloatv(GL_PROJECTION_MATRIX, pr_mat);
	glUniformMatrix4fv(this->pr_mat_loc, 1, GL_FALSE, pr_mat);

	delete[] pr_mat;
}

// unbinds the shader program for this Shader class object
void Shader::end()
{
	glUseProgram(0);
}
