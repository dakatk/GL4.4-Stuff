#ifndef MODEL_HPP__
#define MODEL_HPP__

#include "Shader.hpp"

#include <string>

#define NONE -1

using namespace std;

class Model {

	private:
		unsigned int vbo;
		unsigned int tex;

		bool cloned;
		int draw;
		int uuid;

		float theta;
		float phi;
		float x;
		float y;
		float z;

	public:
		Model(string objfile, string texfile, float scale);
		Model(const Model& source);
		~Model();

		Model* clone() const { return new Model(*this); }

		void setUUID(int uuid);
		int getUUID();

		void rotateTo(float theta, float phi);
		void moveTo(float x, float y, float z);		
		void render(Shader* shader);
		void select(Shader* shader);

		float getTheta();
		float getPhi();
		float getX();
		float getY();
		float getZ();
};

#endif
