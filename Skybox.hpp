#ifndef SKYBOX_HPP__
#define SKYBOX_HPP__

#include "Shader.hpp"

#include <string>

using namespace std;

class Skybox {

	private:
		unsigned int vbo;
		unsigned int tex;

	public:
		Skybox(string left, string right, string top,
				string bottom, string front, string back);
		~Skybox();

		void render(Shader* shader);
};

#endif
