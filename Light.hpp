#ifndef LIGHT_HPP__
#define LIGHT_HPP__

class Light {

	private:
		float* ambient;
		float* diffuse;
		float* specular;
		float* pos;

	public:
		Light(float x, float y, float z);
		~Light();

		void setSpecular(float r, float g, float b);
		void setAmbient(float r, float g, float b);
		void setDiffuse(float r, float g, float b);

		float* getSpecular();
		float* getAmbient();
		float* getDiffuse();
		float* getPos();				
};

#endif
