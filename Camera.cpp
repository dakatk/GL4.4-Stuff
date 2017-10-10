#include "Camera.hpp"

#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <string>

#include <cstring>
#include <cmath>

#define AZIMUTH_RANGE 80.0f

using namespace std;

// sets up the Camera object with preliminary values
Camera::Camera(float x, float y, float z)
{
	this->theta = 0.0f;
	this->phi = 0.0f;

	this->x = x;
	this->y = y;
	this->z = z;
}

// checks if a Model object has been picked by the
// camera within a certain distance. This is where
// the 'picking' shader set is primarily used
bool Camera::picked(Model* model, Shader* shader, double distance)
{
	unsigned char res[4];
    int viewport[4];

	unsigned int hit;

	glGetIntegerv(GL_VIEWPORT, viewport);

	shader->begin();

	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	shader->setProjectionMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluPerspective(45.0, (double)viewport[2] / (double)viewport[3], 0.1, distance);
	glOrtho(0.0, 8.0, 0.0, 8.0, 0.1, distance);

	this->doRotate();
	this->doTranslate();

	model->select(shader);

	glReadPixels(viewport[2] / 2, viewport[3] / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res);
	hit = (unsigned int)(res[0]);

	glPopMatrix();
	
	shader->end();

	return (unsigned int)(model->getUUID()) == hit;
}

// move the camera's position based on the
// current look rotation about the Y-axis (theta)
void Camera::move(float dx, float dy, float dz)
{
	double x_comp = sin(this->theta * M_PI / 180.0f);
	double z_comp = -cos(this->theta * M_PI / 180.0f);

	double x_norm = sin((this->theta + 90.0f) * M_PI / 180.0f);
	double z_norm = -cos((this->theta + 90.0f) * M_PI / 180.0f);

	this->x += (dz * x_comp) + (dx * x_norm);
	this->z += (dz * z_comp) + (dx * z_norm);
	this->y += dy;
}

// moves the camera immediately to a position
void Camera::moveTo(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// rotates the camera by given rates, forces
// vertical rotation to stay within certain
// boundaries
void Camera::rotate(float dtheta, float dphi)
{
	this->theta += dtheta;
	this->phi += dphi;

	if(this->phi > AZIMUTH_RANGE)
		this->phi = AZIMUTH_RANGE;

	else if(this->phi < -AZIMUTH_RANGE)
		this->phi = -AZIMUTH_RANGE;
}

// wraps 'glTranslatef' into a Camera object
// function (used primarily for neatness)
void Camera::doTranslate()
{
	glTranslatef(this->x, this->y, this->z);
}

// performs necessary 'glRotatef' calculations
// based on the camera's horizontal and
// vertical rotation values (theta and phi,
// respectively)
void Camera::doRotate()
{
	float rad = (this->theta * M_PI) / 180.0;

	glRotatef(this->theta, 0.0f, 1.0f, 0.0f);
	glRotatef(-(this->phi), cos(rad), 0.0f, sin(rad));
}

// returns the camera's 'X' position
float Camera::getX()
{
	return this->x;
}

// returns the camera's 'Y' position
float Camera::getY()
{
	return this->y;
}

// returns the camera's 'Z' position
float Camera::getZ()
{
	return this->z;
}
