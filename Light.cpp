#include "Light.hpp"

// initializes all arrays for the Light object,
// sets preliminary values for position
Light::Light(float x, float y, float z)
{
	this->specular = new float[3];
	this->ambient = new float[3];
	this->diffuse = new float[3];
	this->pos = new float[3];
	
	this->pos[0] = x;
	this->pos[1] = y;
	this->pos[2] = z;
}

// frees each array for the Light object
Light::~Light()
{
	delete[] specular;
	delete[] ambient;
	delete[] diffuse;
	delete[] pos;
}

// stores r, g, and b values (each from 1.0 - 0.0)
// into the Light object's specular color array
void Light::setSpecular(float r, float g, float b)
{
	this->specular[0] = r;
	this->specular[1] = g;
	this->specular[2] = b;
}

// stores r, g, and b values (each from 1.0 - 0.0)
// into the Light object's ambient color array
void Light::setAmbient(float r, float g, float b)
{
	this->ambient[0] = r;
	this->ambient[1] = g;
	this->ambient[2] = b;
}

// stores r, g, and b values (each from 1.0 - 0.0)
// into the Light object's diffuse color array
void Light::setDiffuse(float r, float g, float b)
{
	this->diffuse[0] = r;
	this->diffuse[1] = g;
	this->diffuse[2] = b;
}

// returns the Light object's specular color array
float* Light::getSpecular()
{
	return this->specular;
}

// returns the Light object's ambient color array
float* Light::getAmbient()
{
	return this->ambient;
}

// returns the Light object's diffuse color array
float* Light::getDiffuse()
{
	return this->diffuse;
}

// returns the Light object's position array
float* Light::getPos()
{
	return this->pos;
}
