#pragma once
/*
@class ModelBRDF

@brief Render a 3D model with BRDF material. 

The class renders a 3D model with BRDF reflection models. 
The BRDF shader code is part of the C++ file. 
Create an instance, load an obj model, and set the material parameters.

Note that the class as part of setforge outputs linear depth values. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Feb 22, 2019

MIT License
------------------------------------------------------------------------------
Last edited:

May 9, 2020, RR
- Adapted the shader code to output linear depth values. 

*/

// stl
#include <iostream>
#include <string>
#include <vector>


// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// local
#include "ModelOBJ.h"
#include "BRDFTypes.h"

namespace cs557
{

class ModelBRDF : public cs557::OBJModel
{
public:

		/*
		Load an OBJ model from file
		@param path_and_filename - number of rows
		@param shader_program - overwrite the default shader program by passing a hander to the constructor
		*/
		void create(string path_and_filename, BRDFMaterial& material, int shader_program = -1);


		/*
		Draw the obj model
		@param viewMatrix - a view matrix object
		@param modelMatrix - a model matrix object.
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix);
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);





private:

		// the brdf material
		BRDFMaterial		_brdf_material;
};


}//namespace cs557