#include "Model.hpp"

#include <SOIL/SOIL.h>
#include <GL/glew.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <iostream>

// used to keep individual values for each
// vertex neatly packed and organized
struct Vertex {

	float x;
	float y;
	float z;
	float u;
	float v;
	float nx;
	float ny;
	float nz;
};

// sets the number each type of value found
// in the OBJ file to their respectve pointers
// (I.E., 'vcount' stores the number of vertices)
static void _getComponentAmounts(const char* filename, int* vcount, int* tcount, int* ncount, int* icount)
{
	FILE* fp = fopen(filename, "r");
	int _vcount, _tcount, _ncount, _icount;
	_vcount = _tcount = _ncount = _icount = 0;

	for(;;)
	{
		char lineheader[128];

		int res = fscanf(fp, "%s", lineheader);
		if(res == EOF)
			break;

		if(strcmp(lineheader, "v") == 0)
			_vcount ++;
		else if(strcmp(lineheader, "vt") == 0)
			_tcount ++;
		else if(strcmp(lineheader, "vn") == 0)
			_ncount ++;
		else if(strcmp(lineheader, "f") == 0)
			_icount ++;
	}
	fclose(fp);

	*vcount = _vcount;
	*tcount = _tcount;
	*ncount = _ncount;
	*icount = _icount * 3;
}

// loads values from an OBJ file to an array of vertices
static struct Vertex* _loadOBJ(const char* objfile, float scale, int* amount)
{
	struct Point3f {

		float x;
		float y;
		float z;
	};

	struct Coord2f {

		float u;
		float v;
	};

	int vsize, tsize, nsize, isize;
	_getComponentAmounts(objfile, &vsize, &tsize, &nsize, &isize);

	Vertex* vertices = (struct Vertex*)calloc(isize, sizeof(struct Vertex));
	Point3f* verts = (struct Point3f*)calloc(vsize, sizeof(struct Point3f));
	Coord2f* texs = (struct Coord2f*)calloc(tsize, sizeof(struct Coord2f));
	Point3f* norms = (struct Point3f*)calloc(nsize, sizeof(struct Point3f));

	int* vindices = (int*)calloc(isize, sizeof(int));
	int* tindices = (int*)calloc(isize, sizeof(int));
	int* nindices = (int*)calloc(isize, sizeof(int));

	FILE* file = fopen(objfile, "r");

	if(file == NULL)
	{
		fprintf(stderr, "File %s does not exist", objfile);
		return NULL;
	}
	int vert_amt, norm_amt, tex_amt, ind_amt, line;
	vert_amt = norm_amt = tex_amt = ind_amt = 0;

	for(line = 0;;line ++)
	{
		char lineheader[128];
		int res = fscanf(file, "%s", lineheader);

		if(res == EOF)
			break;

		if(strcmp(lineheader, "v") == 0)
		{
			struct Point3f v;
			int result;

			result = fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
			if(result != 3)
				cout << "Warning: incomplete vertex at line " << to_string(line) << endl;

			verts[vert_amt ++] = v;
		}

		else if(strcmp(lineheader, "vt") == 0)
		{
			struct Coord2f t;
			int result;

			result = fscanf(file, "%f %f\n", &t.u, &t.v);
			if(result != 2)
				cout << "Warning: incomplete texcoord at line " << to_string(line) << endl;

			texs[tex_amt ++] = t;
		}

		else if(strcmp(lineheader, "vn") == 0)
		{
			struct Point3f n;
			int result;

			result = fscanf(file, "%f %f %f\n", &n.x, &n.y, &n.z);
			if(result != 3)
				cout << "Warning: incomplete normal at line " << to_string(line) << endl;

			norms[norm_amt ++] = n;
		}

		else if(strcmp(lineheader, "f") == 0)
		{
			int vertexIndex[3], texIndex[3], normalIndex[3], result;

			memset(vertexIndex, 0, 3);
			memset(texIndex, 0, 3);
			memset(normalIndex, 0, 3);

			result = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &texIndex[0], &normalIndex[0],
				&vertexIndex[1], &texIndex[1], &normalIndex[1],
				&vertexIndex[2], &texIndex[2], &normalIndex[2]);
		
			if(result != 9)
				cout << "Warning: incomplete face at line " << to_string(line) << endl;


			vindices[ind_amt] = vertexIndex[0] - 1;
			vindices[ind_amt + 1] = vertexIndex[1] - 1;
			vindices[ind_amt + 2] = vertexIndex[2] - 1;
			tindices[ind_amt] = texIndex[0] - 1;
			tindices[ind_amt + 1] = texIndex[1] - 1;
			tindices[ind_amt + 2] = texIndex[2] - 1;
			nindices[ind_amt] = normalIndex[0] - 1;
			nindices[ind_amt + 1] = normalIndex[1] - 1;
			nindices[ind_amt + 2] = normalIndex[2] - 1;
			  
			ind_amt += 3;
		}
	}

	int i, nverts = 0;
	for(i = 0; i < ind_amt; i ++)
	{
		struct Point3f _v = verts[(vindices[i])];
		struct Coord2f _t = texs[(tindices[i])];
		struct Point3f _n = norms[(nindices[i])];

		_v.x *= scale;
		_v.y *= scale;
		_v.z *= scale;

		_n.x *= scale;
		_n.y *= scale;
		_n.z *= scale;

		Vertex _vertex = {.x=_v.x, .y=_v.y, .z=_v.z, .u=_t.u, .v=1.0f-(_t.v), .nx=_n.x, .ny=_n.y, .nz=_n.z};
		vertices[nverts ++] = _vertex;
	}
	*amount = nverts;

	free(verts);
	free(texs);
	free(norms);
	free(vindices);
	free(tindices);
	free(nindices);
	fclose(file);

	return vertices;
}

Model::Model(string objfile, string texfile, float scale)
{
	this->theta = 0.0f;
	this->phi = 0.0f;
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;

	glGenTextures(1, &(this->tex));
	glBindTexture(GL_TEXTURE_2D, this->tex);

	unsigned char* image;
	int tex_width, tex_height;

	image = SOIL_load_image(texfile.c_str(), &tex_width, &tex_height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glGenerateMipmapEXT(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	struct Vertex* vertices;
	vertices = _loadOBJ(objfile.c_str(), scale, &(this->draw));

	glGenBuffers(1, &(this->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, (this->draw) * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	free(vertices);

	this->moveTo(0.0f, 0.0f, 0.0f);
	this->rotateTo(0.0f, 0.0f);

	this->cloned = false;
	this->uuid = NONE;
}

Model::Model(const Model& source)
{
	this->vbo = source.vbo;
	this->tex = source.tex;
	
	this->draw = source.draw;
	this->uuid = source.uuid;
		
	this->theta = source.theta;
	this->phi = source.phi;

	this->x = source.x;
	this->y = source.y;
	this->z = source.z;

	this->cloned = true;
}

Model::~Model()
{
	if(!(this->cloned))
	{
		glDeleteBuffers(1, &(this->vbo));
		glDeleteTextures(1, &(this->tex));
	}
}

void Model::rotateTo(float theta, float phi)
{
	this->theta = theta;
	this->phi = phi;
}

void Model::moveTo(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Model::setUUID(int uuid)
{
	this->uuid = uuid;
}

float Model::getTheta()
{
	return this->theta;
}

float Model::getPhi()
{
	return this->phi;
}

float Model::getX()
{
	return this->x;
}

float Model::getY()
{
	return this->y;
}

float Model::getZ()
{
	return this->z;
}

int Model::getUUID()
{
	return this->uuid;
}
		
void Model::render(Shader* shader)
{
	glPushMatrix();
	glTranslatef(this->x, this->y, this->z);

	float rad = (this->theta * M_PI) / 180.0;

	glRotatef(this->theta, 0.0f, 1.0f, 0.0f);
	glRotatef(-(this->phi), cos(rad), 0.0f, sin(rad));

	shader->setModelViewMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindTexture(GL_TEXTURE_2D, this->tex);

	glEnableVertexAttribArray(NORMAL_ATTR);
	glEnableVertexAttribArray(POSITION_ATTR);
	glEnableVertexAttribArray(TEXCOORD_2D_ATTR);
	glDisableVertexAttribArray(TEXCOORD_CUBE_ATTR);

	glVertexAttribPointer(POSITION_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), NULL);
	glVertexAttribPointer(TEXCOORD_2D_ATTR, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(Vertex, u));
	glVertexAttribPointer(NORMAL_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(Vertex, nx));

	glDrawArrays(GL_TRIANGLES, 0, this->draw);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void Model::select(Shader* shader)
{
	if(this->uuid == NONE) return;

	shader->setUniformi("uuid", this->uuid);

	glPushMatrix();
	glTranslatef(this->x, this->y, this->z);

	float rad = (this->theta * M_PI) / 180.0;

	glRotatef(this->theta, 0.0f, 1.0f, 0.0f);
	glRotatef(-(this->phi), cos(rad), 0.0f, sin(rad));

	shader->setModelViewMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	glDisableVertexAttribArray(NORMAL_ATTR);
	glEnableVertexAttribArray(POSITION_ATTR);
	glDisableVertexAttribArray(TEXCOORD_2D_ATTR);
	glDisableVertexAttribArray(TEXCOORD_CUBE_ATTR);

	glVertexAttribPointer(POSITION_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), NULL);

	glDrawArrays(GL_TRIANGLES, 0, this->draw);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
}
