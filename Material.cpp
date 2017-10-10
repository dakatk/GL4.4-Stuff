#include "Material.hpp"

// initializes the Material object's color arrays.
// sets the shininess to the given value (should
// be a power of 2 for best results)
Material::Material(float s)
{
	this->ambient = new float[3];
	this->diffuse = new float[3];
	this->specular = new float[3];

	this->shininess = s;
}

// frees all of the Material object's arrays
Material::~Material()
{
	delete[] this->ambient;
	delete[] this->diffuse;
	delete[] this->specular;
}

void Material::setAmbient(float r, float g, float b)
{
	this->ambient[0] = r;
	this->ambient[1] = g;
	this->ambient[2] = b;
}

void Material::setDiffuse(float r, float g, float b)
{
	this->diffuse[0] = r;
	this->diffuse[1] = g;
	this->diffuse[2] = b;
}

void Material::setSpecular(float r, float g, float b)
{
	this->specular[0] = r;
	this->specular[1] = g;
	this->specular[2] = b;
}

float* Material::getAmbient()
{
	return this->ambient;
}

float* Material::getDiffuse()
{
	return this->diffuse;
}

float* Material::getSpecular()
{
	return this->specular;
}

float Material::getShininess()
{
	return this->shininess;
}

