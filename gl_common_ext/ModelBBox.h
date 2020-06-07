#pragma once
/*
@class BBox
@brief The class renders a wire frame bounding box.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
(515) 294 7044
rafael@iastate.edu
MIT License
----------------------------------------------------------------------------------------------------------------------
Last edits:

June 6, 2020, RR
- Added an api to return the bounding box corner points. 

*/

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// local
#include "VertexBuffers.h"			// create vertex buffer object
#include "ShaderProgram.h"			// create a shader program


using namespace std;


namespace cs557
{

	class BBox {
	
	public:
		/*
		Create a simple coordinate system in the centroid
		@param length - the length of each unit axis
		*/
		void create(glm::vec3 center, glm::vec3 edges, int shader_program = -1);


		/*
		Draw the coordinate system
		@param viewMatrix - a view matrix object
		@param modelMatrix - a model matrix object.
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix);

		/*
		Return the shader program
		@return - int containing the shader program
		*/
		int getProgram(void){return program;}


		/*
		Return the bounding box corners. 
		*/
		std::vector<glm::vec3> getCorners(void){return _bb_corners;}

	private:


		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		
		// the shader program that renders this object
		int program;


		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;


		float _width;
		float _height; 

		// the bounding box corners.
		std::vector<glm::vec3>	_bb_corners;
	};
}