#pragma once
/*
class Model3D

This class renders a 3D model (RGB), a normal map, and a depth map (linear) into an fbo,
and saves the output into files. 
Note that the viewmatrix as well as the intrinsic parameters must be set correctly. 
The scene will not work correctly otherwise. 

The class creates a light source for an object. 
The light source is attached to the camera, thus, simulates a uniform distributed light. 
The light is updated in 'setCameraMatrix'

Features:
- Renders an RGB image of the model into a fbo color attachment (RGB8, CV_8UC3)
- Renders a normal map of the model into an fbo color attachment as float (GL_RGBA32F_ARB, CV_32FC1)
- Renders a depth map (linearized) into a fbo depth attachment as float (GL_DEPTH_COMPONENT32, CV_32FC1).
- Writes the images to files (.png)

Usage:
renderer = new ModelRenderer(1280, 1024, 1280, 1024);
renderer->setVerbose(true); // set first to get all the output info
renderer->setModel("path to model");
renderer->setOutputPath("output");

Use 'draw_and_save()' to draw and save the model. 
Use 'draw()' to only draw the model. 
Note that an OpenGL window context must be ready. 

Use 'enable_writer(true/false)' to enable/disable the file writer. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved
--------------------------------------------------------------------------
last edited:

*/

// stl
#include <iostream>
#include <vector>
#include <string>



// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions


// local
#include "ModelOBJ.h"
#include "ShaderProgram.h"
#include "CommonTypes.h"  
#include "ModelCoordinateSystem.h"


using namespace std;

class Model3D {

public:
	/*
	Constructor
	*/
	Model3D();
	~Model3D();


	/*
	Create the model instance.
	@param path_and_file - string containg the relative or absolute path to the model.
	@return - true, if model was successfully loaded.
	*/
	bool create(string path_and_file);


	/*
	Draw the model 
	@param vm -  a glm 4x4 view matrix
	*/
	bool draw(glm::mat4& vm);

private:

	// members

	// the model to render
	cs557::OBJModel*			_obj_model;

	glm::mat4				_projectionMatrix;		
	glm::mat4				_viewMatrix;			
	glm::mat4               _modelMatrix; 


	// The light source
	cs557::LightSource		_light0;
	cs557::LightSource		_light1;
	cs557::LightSource		_light2;

	// Material
	cs557::Material	 		_mat0;

	// a corodinate system
	cs557::CoordinateSystem     _coordinateSystem;
	glm::mat4                   _modelMatrixCoordSystem; // for the coordinate system


};