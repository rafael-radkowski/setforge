/*

//../data/stanford_bunny_02_rot.obj  -img_w 1280 -img_h 1024 -m SPHERE  -seg 5 -rows 5 -rad 1.3 -verbose

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved

---------------------------------
last edited:
Jan 21, 2019: added a 3D model to the polyhedron
*/

#include <iostream>
#include <string>


// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// glfw includes
#include <GLFW/glfw3.h>

// local
#include "Window.h" // the windows
#include "OpenGLDefaults.h" // some open gl and glew defaults
#include "VertexBuffers.h"  // create vertex buffer object
#include "ShaderProgram.h"  // create a shader program
#include "CommonTypes.h"  // types that all projects use
#include "ModelCoordinateSystem.h"
#include "Polyhedron.h"
#include "ModelOBJ.h" // to render a 3D model

using namespace cs557;
using namespace arlab;

//------------------------------------------------------------
//
//	Some global variables to keep track of

// The handle to the window object
GLFWwindow *window = NULL;

// Transformation pipeline variables
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;       // Store the view matrix


glm::mat4				coord_mm; 
CoordinateSystem			coord;

// the polyhedron geometry
Polyhedron				poly;

// Set up our green background color
GLfloat clear_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat clear_depth[] = {1.0f, 1.0f, 1.0f, 1.0f};


//------------------------------------------------------------
//
//	Models

cs557::OBJModel			obj_model;
cs557::LightSource		light0;
cs557::LightSource		light1;
cs557::Material	 		mat0;

using namespace std;
using namespace cs557;


void InitWindow(void)
{

	//cout << _MSC_VER << endl;


	// Init the GLFW Window
    window = cs557::initWindow("Render to Image");

    // Initialize the GLEW apis
    cs557::initGlew();

}


void InitRenderer(void)
{

	
	projectionMatrix = glm::perspective(1.2f, (float)800 / (float)600, 0.1f, 10.f);
	viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0, 2.3f), glm::vec3(0.0f, 0.0f, 00.f), glm::vec3(0.0f, 1.0f, 0.0f));
	coord_mm = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 


	int program = cs557::LoadAndCreateShaderProgram("../src/lit_scene.vs", "../src/lit_scene.fs");

	obj_model.create("../data/stanford_bunny_02_rot.obj", program);
	poly.create(4);
	coord.create(25); 

	light0.pos = glm::vec3(0.0f, -3.0f, 3.0f);
	light0.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	light0.k1 = 0.1;
	light0.index = 0;
	
	light1.pos = glm::vec3(0.0f, 3.0f, 3.0f);
	light1.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	light1.k1 = 0.1;
	light1.index = 1;


	mat0.diffuse_mat = glm::vec3(1.0, 0.0, 0.0);
	mat0.diffuse_int = 0.8;
	mat0.ambient_mat = glm::vec3(1.0, 0.0, 0.0);
	mat0.ambient_int = 0.2;
	mat0.specular_mat = glm::vec3(1.0, 1.0, 1.0);
	mat0.specular_int = 0.2;
	mat0.specular_s = 6.0;

	mat0.apply(obj_model.getProgram());
	light1.apply(obj_model.getProgram());
	light0.apply(obj_model.getProgram());

}


void DrawLoop(void)
{
    // Enable depth test
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_BLEND); 
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	 // Init the view matrix. 
    cs557::InitControlsViewMatrix(viewMatrix);

    while (!glfwWindowShouldClose(window))
    {
        // Clear the entire buffer with our green color (sets the background to be green).
        glClearBufferfv(GL_COLOR, 0, clear_color);
        glClearBufferfv(GL_DEPTH, 0, clear_depth);

        
        //----------------------------------------------------------------------------------------------------------------------------
        // update values and draw
		glm::mat4 rotView = GetCamera().getViewMatrix();

		poly.draw(projectionMatrix, rotView, coord_mm);
		coord.draw(projectionMatrix, rotView, coord_mm);
		obj_model.draw(projectionMatrix, rotView, coord_mm);
        //----------------------------------------------------------------------------------------------------------------------------

        // Swap the buffers so that what we drew will appear on the screen.
        glfwSwapBuffers(window);
        glfwPollEvents();


    }
}






int main(int argc, char** argv)
{


	// Init the output window
	InitWindow();

	// Init the image renderer 
	InitRenderer();

	// Start rendering
	DrawLoop();


	return 1;
}