#ifndef MATERIAL_HPP__
#define MATERIAL_HPP__

class Material {

	private:
		float* ambient;
		float* diffuse;
		float* specular;

		float shininess;
		
	public:
		Material(float s);
		~Material();

		void setAmbient(float r, float g, float b);
		void setDiffuse(float r, float g, float b);
		void setSpecular(float r, float g, float b);

		float* getAmbient();
		float* getDiffuse();
		float* getSpecular();
		float getShininess();
};

#endif
