#ifndef SHADER_HPP__
#define SHADER_HPP__

#include "Material.hpp"
#include "Light.hpp"

#include <string>

#define MODELVIEW_MATRIX_STR "modelViewMatrix"
#define PROJECTION_MATRIX_STR "projMatrix"
#define CAMERA_MATRIX_STR "viewMatrix"

#define TEXCOORD_CUBE_STR "texcoordCube"
#define TEXCOORD_2D_STR "texcoord2D"
#define POSITION_STR "position"
#define NORMAL_STR "normal"

#define TEXCOORD_CUBE_ATTR 3
#define TEXCOORD_2D_ATTR 1
#define POSITION_ATTR 0
#define NORMAL_ATTR 2

#define TEXTURE_CUBE_ID 1
#define TEXTURE_2D_ID 0

#define TEXTURE_CUBE_STR "texCube"
#define TEXTURE_2D_STR "tex2D"

#define FRAG_COLOR_STR "gl_FragColor"
#define NUM_LIGHTS_STR "num_lights"
#define CAMERA_POS_STR "cameraPos"
#define IS_SKYBOX_STR "is_skybox"
#define PICKED_STR "picked"

using namespace std;

class Shader {

	private:
		unsigned int mv_mat_loc;
		unsigned int pr_mat_loc;
		unsigned int cm_mat_loc;
		unsigned int prog_id;

		int num_lights;

	public:
		Shader(string vertfile, string fragfile);
		~Shader();
	
		void setProjectionMatrix();
		void setModelViewMatrix();
		void setCameraMatrix();

		void setLighting(Light** light, int amount);
		void setMaterial(Material* material);
		void setLight(Light* light, int n);

		void setUniformi(string name, int value);
		void setTexture(int num);

		void begin();
		void end();
};

#endif
