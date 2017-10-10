#include "Skybox.hpp"

#include <SOIL/SOIL.h>
#include <GL/glew.h>

#include <iostream>
#include <cstring>
#include <string>

#define NUM_VERTS 36

// used to keep values packed and
// neatly organized
struct Vertex {

	float x;
	float y;
	float z;
	float t;
	float u;
	float v;
};

// only initializes a Vertex struct type defined in this
// file, used primarily for neatness of the code
static struct Vertex createVertex3d(float x, float y, float z)
{
	struct Vertex _vertex = {.x=x, .y=y, .z=z, .t=x, .u=y, .v=z};
	return _vertex;
}

// creates a Skybox class object by loading multiple
// texture files to a cube map texture ID. Also creates
// a VBO using pre-defined vertices, packed as
// Vertex structs
Skybox::Skybox(string left, string right, string top,
				string bottom, string front, string back)
{
	glGenTextures(1, &(this->tex));
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->tex);

	const char* filenames[] = {
		right.c_str(), left.c_str(), top.c_str(), 
		bottom.c_str(), back.c_str(), front.c_str()
	};

	int i;
	for(i = 0; i < 6; i ++)
	{
		unsigned char* image;
		int tex_width, tex_height;

		image = SOIL_load_image(filenames[i], &tex_width, &tex_height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (unsigned int)i, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	struct Vertex* skybox_verts = new struct Vertex[NUM_VERTS]
	{
		createVertex3d(-1.0,  1.0, -1.0),
		createVertex3d(-1.0, -1.0, -1.0),
		createVertex3d(1.0, -1.0, -1.0),
		createVertex3d(1.0, -1.0, -1.0),
		createVertex3d(1.0,  1.0, -1.0),
		createVertex3d(-1.0,  1.0, -1.0),

		createVertex3d(-1.0, -1.0,  1.0),
		createVertex3d(-1.0, -1.0, -1.0),
		createVertex3d(-1.0,  1.0, -1.0),
		createVertex3d(-1.0,  1.0, -1.0),
		createVertex3d(-1.0,  1.0,  1.0),
		createVertex3d(-1.0, -1.0,  1.0),

		createVertex3d(1.0, -1.0, -1.0),
		createVertex3d(1.0, -1.0,  1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(1.0,  1.0, -1.0),
		createVertex3d(1.0, -1.0, -1.0),

		createVertex3d(-1.0, -1.0,  1.0),
		createVertex3d(-1.0,  1.0,  1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(1.0, -1.0,  1.0),
		createVertex3d(-1.0, -1.0,  1.0),

		createVertex3d(-1.0,  1.0, -1.0),
		createVertex3d(1.0,  1.0, -1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(1.0,  1.0,  1.0),
		createVertex3d(-1.0,  1.0,  1.0),
		createVertex3d(-1.0,  1.0, -1.0),

		createVertex3d(-1.0, -1.0, -1.0),
		createVertex3d(-1.0, -1.0,  1.0),
		createVertex3d(1.0, -1.0, -1.0),
		createVertex3d(1.0, -1.0, -1.0),
		createVertex3d(-1.0, -1.0,  1.0),
		createVertex3d(1.0, -1.0,  1.0)
	};

	glGenBuffers(1, &(this->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * sizeof(struct Vertex), skybox_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] skybox_verts;
}

// destroys the Skybox object's VBO and
// texture ID
Skybox::~Skybox()
{
	glDeleteBuffers(1, &(this->vbo));
	glDeleteTextures(1, &(this->tex));
}

// renders the Skybox object to the scene
// using it's generated VBO and texture ID
// (texture is rendered as a cube map instead
// of a 2D texture, so this differentiates
// slightly from how Model objects are rendered)
void Skybox::render(Shader* shader)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP);

	glPushMatrix();
	glDepthMask(GL_FALSE);

	shader->setModelViewMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->tex);

	glDisableVertexAttribArray(NORMAL_ATTR);
	glEnableVertexAttribArray(POSITION_ATTR);
	glDisableVertexAttribArray(TEXCOORD_2D_ATTR);
	glEnableVertexAttribArray(TEXCOORD_CUBE_ATTR);

	glVertexAttribPointer(POSITION_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), NULL);
	glVertexAttribPointer(TEXCOORD_CUBE_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(Vertex, t));

	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDepthMask(GL_TRUE);
	glPopMatrix();

	glDisable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}
