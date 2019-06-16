#pragma once
/*
class Model3D

This class implements a simple model renderer with the sole purpose to show the object in a window.
It links the shader code with the geometry with a default light source and calls the draw function.

Features:
- Loads an obj file and renders it. 
- Links a shader program to the obj geometry. 
- Adds a default light source, attaches this light source to the camera (viewmatrix) and points it towards
	the 3D model

Usage:
renderer = new Model3D();
renderer->create("path to model");
 
Use 'draw()' to only draw the model. 
Note that an OpenGL window context must be ready. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Feb 19, 2019
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

class Model3DExt {

public:
	/*
	Constructor
	*/
	Model3DExt();
	~Model3DExt();


	/*
	Create the model instance.
	@param path_and_file - string containg the relative or absolute path to the model.
	@return - true, if model was successfully loaded.
	*/
	bool create(string path_and_file);


	/*
	Set the transformation for this object
	*/
	bool setTransformation(glm::mat4 transform);


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