/*
 * Author: Dusten Knull
 *
 * This project is used for testing different features
 * of modern OpenGL (4.0+) using shaders. This program
 * demonstrates the usage of a skybox and of "picking"
 * an object in a scene. Both of these features can be
 * implemented without shaders, however, using shaders
 * helps to streamline the rendering process for both
 * and allows for better visual effects on computers
 * with graphics cards that can handle it.
 */
#include <GL/glew.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <chrono>
#include <cmath>

#include "Material.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Light.hpp"

#define WINDOW_TITLE "GL4.4 Test World"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BOBBING_FREQ 0.012
#define BOBBING_RATE 0.9f

#define PICKING_RANGE 500.0

#define MOUSE_SPEED 0.16f
#define MOVE_SPEED 0.50f
#define NUM_WALLS 161

#define NUM_KEYS 7
#define KEY_ESC 0
#define KEY_Q 1
#define KEY_W 2
#define KEY_E 3
#define KEY_A 4
#define KEY_S 5
#define KEY_D 6

#define NUM_BTNS 2
#define BTN_L 0
#define BTN_R 1

/*
TODO:
 - Add physical boundaries for box and player
*/

using namespace std;
using namespace chrono;

SDL_GLContext main_context;
SDL_Window* main_window;

Camera* camera;
Skybox* skybox;

Shader* selector;
Shader* shader;

Material* stone;
Material* wood;

Model* walls[NUM_WALLS];
Model* wall;
Model* box;

bool buttons[NUM_BTNS];
bool keys[NUM_KEYS];

bool running = true;
bool picked = false;

double getElapsedBobbingTime(void);
double getElapsedGameTime(void);

bool initSDL(void);
bool initWindow(void);
bool initContext(int, int);

void startGL(void);
void runTest(void);
void cleanup(void);

int main(int argc, char* argv[])
{
	if(!initSDL())
		return 1;

	if(!initWindow())
		return 1;
	
	main_context = SDL_GL_CreateContext(main_window);

	if(!initContext(4, 4))
		return 1;

	// traps mouse and ensures SDL knows to swap buffers
	// each frame
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GL_SetSwapInterval(1);

	startGL();

	return 0;
}

// attempts to init SDL
bool initSDL()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
    	cout << "Failed to init SDL" << endl;
    	return false;
	}
	return true;
}

// creates a GL context based on SDL functions for doing so.
// returns false if context creation fails
bool initContext(int major, int minor)
{
	bool setCore = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0;
	bool setMajor = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major) == 0;
	bool setMinor = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor) == 0;
	bool setDouble = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0;

	bool allSet = setCore && setMajor && setMinor && setDouble;
	if(!allSet)
	{
		cout << "Failed to create GL context" << endl;
		return false;
	}
	return true;
}

bool initWindow()
{
	main_window = SDL_CreateWindow(

    	WINDOW_TITLE, 
    	SDL_WINDOWPOS_CENTERED,
    	SDL_WINDOWPOS_CENTERED,
    	WINDOW_WIDTH,
    	WINDOW_HEIGHT,
    	SDL_WINDOW_OPENGL
	);
	
	if(!main_window)
	{
		cout << "Failed to create SDL window" << endl;
		return false;
	}
	return true;
}

// loads all models and model textures into GL
// objects (via texture IDs and VBOs). Sets all
// default positions and rotations for these objects,
// also sets up the box to be used when picking checks
// are performed during the main loop
void createObjects()
{
	box = new Model("res/box.obj", "res/box.png", 15.0f);
	box->moveTo(30.0f, 10.0f, 30.0f);
	box->setUUID(1);

	int i, j, index = 0;
	float wall_scale = 50.0f;

	wall = new Model("res/wall.obj", "res/wall.png", wall_scale);
	
	for(i = -3; i <= 3; i ++)
	{
		float x, y, z;

		wall->rotateTo(0.0f, 0.0f);
		for(j = 0; j < 4; j ++)
		{
			x = (float)i * wall_scale;
			y = (wall_scale / 10.0f) + (wall_scale * (float)j);
			z = (-3.0f * wall_scale);

			wall->moveTo(x, y, z);
			walls[index ++] = wall->clone();

			wall->moveTo(x, y, z + (wall_scale * 7.1f));
			walls[index ++] = wall->clone();
		}

		wall->rotateTo(-90.0f, 0.0f);
		for(j = 0; j < 4; j ++)
		{
			x = (-3.55f * wall_scale);
			y = (wall_scale / 10.0f) + (wall_scale * (float)j);
			z = (wall_scale * 0.55f) + (wall_scale * (float)i);

			wall->moveTo(x, y, z);
			walls[index ++] = wall->clone();

			wall->moveTo(x + (wall_scale * 7.1f), y, z);
			walls[index ++] = wall->clone();
		}
		
		wall->rotateTo(0.0f, -90.0f);
		for(j = -3; j <= 3; j ++)
		{
			x = (float)i * wall_scale;
			y = (-wall_scale / 2.0f) + (0.05f * wall_scale);
			z = (wall_scale / 2.0f) + (wall_scale * 0.05f) + (wall_scale * (float)j);

			wall->moveTo(x, y, z);
			walls[index ++] = wall->clone();
		}
	}
}

// creates instances for all Material and Light
// class objects to be used in the scene via
// calculations performed in the main shader set
void createLighting()
{
	stone = new Material(32.0f);
	stone->setAmbient(0.4f, 0.4f, 0.4f);
	stone->setDiffuse(0.4f, 0.4f, 0.4f);
	stone->setSpecular(0.0f, 0.0f, 0.0f);

	wood = new Material(16.0f);
	wood->setAmbient(0.74f, 0.58f, 0.45f);
	wood->setDiffuse(0.74f, 0.58f, 0.45f);
	wood->setSpecular(0.3f, 0.3f, 0.3f);

	Light* light0 = new Light(0.0f, 50.0f, 0.0f);
	light0->setAmbient(0.5f, 0.5f, 0.5f);
	light0->setDiffuse(0.5f, 0.5f, 0.5f);
	light0->setSpecular(1.0f, 1.0f, 1.0f);

	Light* light1 = new Light(20.0f, 20.0f, 20.0f);
	light1->setAmbient(0.6f, 0.6f, 0.6f);
	light1->setDiffuse(0.4f, 0.4f, 0.4f);
	light1->setSpecular(1.0f, 1.0f, 1.0f);

	shader->begin();

	shader->setLight(light0, 0);
	shader->setLight(light1, 1);

	shader->end();

	delete light0;
}

// initializes GLEW and all necessary values
// for the GL pipeline for proper rendering.
// Creates instances for shader (main and picking),
// skybox, and camera
void startGL()
{
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{	
		cout << "Failed to initialize GLEW" << endl;
		exit(1);
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);

	camera = new Camera(0.0f, BOBBING_RATE, -10.0f);
	shader = new Shader("res/main.vs", "res/main.fs");
	selector = new Shader("res/picking.vs", "res/picking.fs");

	skybox = new Skybox("res/lake1_lf.png", "res/lake1_rt.png",
						"res/lake1_up.png", "res/lake1_dn.png",
						"res/lake1_ft.png", "res/lake1_bk.png");
	createLighting();
	createObjects();

	runTest();
	cleanup();
}

// checks for all keyboard and mouse updates.
// preliminary values for player movement and
// look rotation are set and calulcated. Picking
// value is updated if necessary
void update()
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
			running = false;

		else if(e.type == SDL_KEYDOWN)
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					keys[KEY_ESC] = true;
					break;

				case SDLK_q:
					keys[KEY_Q] = true;
					break;

				case SDLK_w:
					keys[KEY_W] = true;
					break;

				case SDLK_e:
					keys[KEY_E] = true;
					break;

				case SDLK_a:
					keys[KEY_A] = true;
					break;

				case SDLK_s:
					keys[KEY_S] = true;
					break;

				case SDLK_d:
					keys[KEY_D] = true;
					break;

				default:
					break;
			}
		}

		else if(e.type == SDL_KEYUP)
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					keys[KEY_ESC] = false;
					break;

				case SDLK_q:
					keys[KEY_Q] = false;
					break;

				case SDLK_w:
					keys[KEY_W] = false;
					break;

				case SDLK_e:
					keys[KEY_E] = false;
					break;

				case SDLK_a:
					keys[KEY_A] = false;
					break;

				case SDLK_s:
					keys[KEY_S] = false;
					break;

				case SDLK_d:
					keys[KEY_D] = false;
					break;

				default:
					break;
			}
		}

		else if(e.type == SDL_MOUSEMOTION)
		{
			float xrel = e.motion.xrel;
			float yrel = e.motion.yrel;

			camera->rotate(xrel * MOUSE_SPEED, -yrel * MOUSE_SPEED);

			if(fabs(xrel) + fabs(yrel) > 0.0f)
				picked = camera->picked(box, selector, PICKING_RANGE);
		}
	}

	if(keys[KEY_ESC])
	{
		cleanup();
		exit(0);
	}

	float dx = 0.0f, dz = 0.0f;

	if(keys[KEY_W] && !keys[KEY_S])
		dz = -MOVE_SPEED;

	else if(keys[KEY_S] && !keys[KEY_W])
		dz = MOVE_SPEED;

	if(keys[KEY_D] && !keys[KEY_A])
		dx = -MOVE_SPEED;

	else if(keys[KEY_A] && !keys[KEY_D])
		dx = MOVE_SPEED;

	if(fabs(dx) + fabs(dz) > 0.0f)
	{
		camera->moveTo(camera->getX(), (getElapsedBobbingTime() * BOBBING_RATE) + BOBBING_RATE, camera->getZ());
		camera->move(dx, 0.0f, dz);

		picked = camera->picked(box, selector, PICKING_RANGE);
	}
}

// renders all objects in the scene and finalizes checks
// for picking, also updates world vieew and position data
// for shaders (modelView and projection matrices)
void render()
{
	shader->begin();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	shader->setProjectionMatrix();
	
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

 	gluPerspective(45.0, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 0.1, 1000.0);

	camera->doRotate();

	shader->setTexture(TEXTURE_CUBE_ID);
	shader->setUniformi(IS_SKYBOX_STR, 1);

	skybox->render(shader);

	camera->doTranslate();
	shader->setCameraMatrix();

	shader->setTexture(TEXTURE_2D_ID);
	shader->setUniformi(IS_SKYBOX_STR, 0);

	shader->setMaterial(wood);

	if(picked)
		shader->setUniformi("picked", 1);

	box->render(shader);
	
	shader->setUniformi("picked", 0);
	shader->setMaterial(stone);

	int i;
	for(i = 0; i < NUM_WALLS; i ++)
		walls[i]->render(shader);
	
	shader->end();
	SDL_GL_SwapWindow(main_window);
}

// returns the number of milliseconds passed since
// the program started
double getElapsedGameTime()
{
	static time_point <system_clock> start = system_clock::now();
	static double e_time = 0.0;

	duration<double> elapsed = system_clock::now() - start;
	e_time = elapsed.count() * 1000.0;

	return e_time;
}

// uses 'getElapsedGameTime' to calculate a time-
// based value to be used for camera bobbing calculations
double getElapsedBobbingTime()
{
	double b_time;
	b_time = sin(getElapsedGameTime() * BOBBING_FREQ);

	return b_time;
}

// run loop that wraps the 'update' and 'render'
// functions into a while loop that runs until
// the program is terminated by the user. Also
// calculates and displays FPS
void runTest()
{
	double start_time, current_time;
	int frames;

	start_time = getElapsedGameTime();
	current_time = 0.0f;
	frames = 0;

	while(running)
	{
		frames ++;
		current_time = getElapsedGameTime();
		if(current_time - start_time >= 1000.0)
		{
			printf("fps: %d\n", frames);
			frames = 0;

			start_time = getElapsedGameTime();
		}

		update();
		render();
	}
}

// clear all class instances from memory accordingly,
// destroy SDL window and GL context
void cleanup()
{
	delete camera;
	delete skybox;
	delete wall;
	delete box;

	int i;
	for(i = 0; i < NUM_WALLS; i ++)
		delete walls[i];

	delete selector;
	delete shader;

	delete stone;
	delete wood;

	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
}
