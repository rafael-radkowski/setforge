/*

//../data/stanford_bunny_02_rot.obj  -img_w 1280 -img_h 1024 -m SPHERE  -seg 5 -rows 5 -rad 1.3 -verbose
// ../data/stanford_bunny_02_rot.obj  -img_w 1280 -img_h 1024 -m POLY -sub 0  -rad 1.3 -verbose

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved
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
#include "ModelRenderer.h"
#include "SphereCoordRenderer.h"
#include "PolyhedronViewRenderer.h"
#include "BalancedPoseTree.h"
#include "ArgParser.h"
#include "types.h"

using namespace cs557;



//------------------------------------------------------------
//
//	Some global variables to keep track of

// The handle to the window object
GLFWwindow *window = NULL;

// Transformation pipeline variables
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix;       // Store the view matrix



// Set up our green background color
GLfloat clear_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat clear_depth[] = {1.0f, 1.0f, 1.0f, 1.0f};


//------------------------------------------------------------
//
//	Models



SphereCoordRenderer*				sphere_renderer = NULL;
PolyhedronViewRenderer*			poly_renderer = NULL;
BalancedPoseTree*				tree_renderer = NULL;

using namespace std;
using namespace cs557;
using namespace arlab;

void InitWindow(void)
{

	//cout << _MSC_VER << endl;


	// Init the GLFW Window
    window = cs557::initWindow("Render to Image");

    // Initialize the GLEW apis
    cs557::initGlew();

}


void InitRenderer(Arguments& opt)
{
     //---------------------------------------------------------
    // Create models

	if (opt.cam == SPHERE) {
		sphere_renderer = new SphereCoordRenderer(opt.windows_width, opt.window_height, opt.image_width, opt.image_height);
		sphere_renderer->setVerbose(opt.verbose); // set first to get all the output info
		sphere_renderer->setModel(opt.model_path_and_file);
		sphere_renderer->setOutputPath(opt.output_path);
		sphere_renderer->createSphereGeometry(opt.camera_distance, opt.segments, opt.rows);
		
	}
	else if (opt.cam == POLY)
	{
		poly_renderer = new PolyhedronViewRenderer(opt.windows_width, opt.window_height, opt.image_width, opt.image_height);
		poly_renderer->setVerbose(opt.verbose); // set first to get all the output info
		poly_renderer->setModel(opt.model_path_and_file);
		poly_renderer->setOutputPath(opt.output_path);
		poly_renderer->create(opt.camera_distance, opt.subdivisions);
		
	}
	else if (opt.cam == TREE)
	{
		tree_renderer = new BalancedPoseTree(opt.windows_width, opt.window_height, opt.image_width, opt.image_height);
		tree_renderer->setVerbose(opt.verbose); // set first to get all the output info
		tree_renderer->setModel(opt.model_path_and_file);
		tree_renderer->setOutputPath(opt.output_path);
		tree_renderer->create(opt.camera_distance, opt.bpt_levels);
	}

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

        

		bool ret = false;
		if(sphere_renderer != NULL)
			ret = sphere_renderer->draw_sequence();

		if (poly_renderer != NULL)
			ret = poly_renderer->draw_sequence();

		if (tree_renderer != NULL)
			ret = tree_renderer->draw_sequence();


        // Swap the buffers so that what we drew will appear on the screen.
        glfwSwapBuffers(window);
        glfwPollEvents();

		if (ret) {
			break;
		}
    }
}






int main(int argc, char** argv) 
{
	cout << "ImageFromModel" << endl;
	cout << "Create RGB color maps, depth images (float), and normal maps (float) from a 3D model \n" << endl;
	cout << "Rafael Radkowski" << endl;
	cout << "Iowa State University" << endl;
	cout << "Rafael@iastate.edu" << endl;
	cout << "Jan 2019, All copyrights reserved." << endl;

	Arguments options = ArgParser::Parse(argc, argv);

	if (options.valid == false) return -1;

	// Init the output window
	InitWindow();

	// Init the image renderer 
	InitRenderer(options);

	// Start rendering
	DrawLoop();

	// The end
	if(sphere_renderer != NULL) delete sphere_renderer;
	if (poly_renderer != NULL) delete poly_renderer;
	if (tree_renderer != NULL) delete tree_renderer;

	return 1;
}