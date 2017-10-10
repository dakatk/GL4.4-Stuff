#ifndef CAMERA_HPP__
#define CAMERA_HPP__

#include "Model.hpp"
#include "Shader.hpp"

class Camera {

	private:
		float theta;
		float phi;
		float x;
		float y;
		float z;
	
	public:
		Camera(float x, float y, float z);

		bool picked(Model* model, Shader* shader, double distance);
		
		void move(float dx, float dy, float dz);
		void moveTo(float x, float y, float z);

		void rotate(float dtheta, float dphi);

		void doTranslate(void);
		void doRotate(void);

		float getX();
		float getY();
		float getZ();
};

#endif
