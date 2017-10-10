# GL4.4-Stuff
#
# Author: Dusten Knull

This project showcases a few modern OpenGL techniques in a rather extensive C++ program. This project is meant solely for 
testing purposes and to show off and try out new concepts in OpenGL development. The project itself should be fairly
self-contained, I included all necessary headers and library files for the project to compile correctly (including GLEW, 
SOIL, and SDL2 libraries and headers). This project was built on and for 32-bit Windows in Eclipse, but with some tweaking
this program should be able to run nicely on a linux environment or 64-bit windows environment. I can't speak to how well 
this program would run on Mac (if at all), because it uses some fairly graphics intensive processes and is based on OpenGL
and GLEW libraries which are not necessarily going to work with Apple's OpenGL framework. 

Features and ideas showcased in this project:
 - Picking an object in a scene
 - Loading GLSL files to be used for rendering
 - Loading data into VBOs to be rendered
 - Creating and displaying a skybox that renders behind everything else
 - Lighting effects using light objects and material color values
 - Camera movement and rotation in a 3D space
 
