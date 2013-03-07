/**
Copyright (C) 2012-2014 Jonas Strandstedt

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Engine.h"
#include "Sphere.h"

gl4::VBO *obj;
gl4::Sphere *sphere;
gl4::Engine *engine;
gl4::DeferredRender *deferredEngine;
glm::vec2 angle;
bool wireframe = false;

int tess;

void keyboardCallback(int key, int state);
void myRenderFunc(void);
void myDeferredRenderFunc(void);
void myInitFunc(void);
void myUpdateFunc(float dt);

int main(int argc, char **argv) {

	// create engine object
	engine = new gl4::Engine(argc, argv);
	
	// set callbacks
	engine->setKeyBoardCallbackfunc(keyboardCallback);
	engine->setUpdateFunc(myUpdateFunc);
	engine->setRenderFunc(myRenderFunc);
	engine->setDeferredRenderFunc(myDeferredRenderFunc);
	engine->setInitFunc(myInitFunc);
	
	// init
	if( ! engine->initGL()) {
		return 1;
	}

	// do render
	engine->render();

	// cleanup
	delete sphere;
	delete obj;
	delete engine;

	// return success
	return 0;
}

void keyboardCallback(int key, int state) 
{
	// increase and decrease tessellation levels
	if(key == 'I' && state == GLFW_PRESS) {
		if(tess < 64)
			tess++;
	}
	if(key == 'O' && state == GLFW_PRESS) {
		if(tess > 1)
			tess--;
	}

	// toggle wireframe
	if(key == 'W' && state == GLFW_PRESS) {
		wireframe = ! wireframe;
	}
}


void myInitFunc(void) 
{
	glEnable (GL_DEPTH_TEST);

	angle[0] = 0.0f;
	angle[1] = 0.0f;
	obj = new gl4::VBO();
	obj->init();

	sphere = new gl4::Sphere(1.0, 30);
	sphere->init();

	tess = 1;

	// load textures
	gl4::TextureManager::getInstance()->loadTexture("earth_diffuse", "data/earth_nasa_lowres.tga");
	gl4::TextureManager::getInstance()->loadTexture("earth_displacement", "data/earth_nasa_topography_lowres.tga");

	deferredEngine = engine->getDeferredRender();
	deferredEngine->addExtendedDeferredShaderFromFile("plane", "data/shaders/DeferredUser.glsl");
}

void myRenderFunc(void) 
{

}

/**
* The deferred renderer is thought of as a "programmable fixed" pipeline.
* The main purpose is not to be fast or efficient but to enable users of 
* the GL4 engine to get the appplication up and running fast without having 
* to create the shaders steps themselves.
*
* The deferred renderer is called before the standard render function and is 
* rendered in two steps. The first is rendering to a multisampled FBO and then 
* rendered on a quad where the light calculations take place. This 
* 
* Please note:
* 	Use 'deferredEngine->bindShader("shadername");' insted of
* 	'ShaderManager::getInstance()->bindShader(shader);' since the deferredEngine 
* 	is binding the neccessary uniforms.
*/
void myDeferredRenderFunc(void) 
{
	
	// draw a colored plane with user defined deferred shader
	deferredEngine->bindShader("plane");

	glm::mat4 plane_transform = glm::scale(glm::mat4(1.0), glm::vec3(5));
	plane_transform = glm::translate(plane_transform,glm::vec3(-0.5,-0.3, 0.0));
	plane_transform = glm::rotate(plane_transform,-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	engine->usePerspectiveProjection(plane_transform);
	obj->render();

	// switch back to default shader and activate textures
	deferredEngine->bindDefaultShader();
	deferredEngine->useState(DEFERRED_TEXTURE, true);

	// bind earth texture
	gl4::TextureManager::getInstance()->bindTexture("earth_diffuse");
	//gl4::TextureManager::getInstance()->bindTexture("earth_displacement", 1);

	// rotate around the x-axis and y-axis
	glm::mat4 transform = glm::rotate(glm::mat4(1.0f),angle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform,angle[0], glm::vec3(1.0f, 0.0f, 0.0f));
	
	engine->usePerspectiveProjection(transform);
	sphere->render();
	
}

void myUpdateFunc(float dt)
{
	if(wireframe)
		deferredEngine->enable(DEFERRED_WIREFRAME);
	else
		deferredEngine->disable(DEFERRED_WIREFRAME);

	float speed = 50.0f;
	if(engine->isKeyPressed(GLFW_KEY_RIGHT)) {
		angle[1] += dt*speed;
	}
	if(engine->isKeyPressed(GLFW_KEY_LEFT)) {
		angle[1] -= dt*speed;
	}
	if(engine->isKeyPressed(GLFW_KEY_UP)) {
		angle[0] += dt*speed;
	}
	if(engine->isKeyPressed(GLFW_KEY_DOWN)) {
		angle[0] -= dt*speed;
	}
}