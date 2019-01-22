#pragma once
/*
Class Polyhedron

Create an Icosahedron and extend it to a Polyhedron by recursive decomposition.

see https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/ for details. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved

---------------------------------------------------------------
Last edited:
Jan 21, 2019: 
- Switched render mode back to fill after the polyheder got rendered. 
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <array>

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
#include "OpenGLDefaults.h" 
#include "VertexBuffers.h" 
#include "ShaderProgram.h" 
#include "PolyhedronGeometry.h"

using namespace std;
using namespace cs557;

namespace arlab {


class Polyhedron
{
private:
		typedef  std::map<std::pair<int, int>, int> Lookup;

		// triangle list, vertex list
		using IndexedMesh=std::pair< vector<glm::vec3>, vector<glm::ivec3> > ;

public:
		/*
		Create an Icosahedron or Polyhedron, depending on the num of subdivisions. 
		@param level - the number of subdivisons. 0 creates an Icosahedron;
		*/
		void create( int level, int shader_program = -1);


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

	private:


		int vertex_for_edge(Lookup& lookup, vector<glm::vec3>& vertices, int first, int second);

		/*
		Subdivide the Icosahedron triangles
		*/
		vector<glm::ivec3> subdivide(vector<glm::vec3>& vertices, vector<glm::ivec3> triangles);

		/*
		Create an Icosahedron
		*/
		void createIcosahedron(void);


		/*
		Create a Polyhedron
		@param subdivisions - the number of subdivisions
		@param vertices - the Icosahedron vertices
		@param triangles - the Icosahedron triangle index list
		*/
		std::pair< vector<glm::vec3>, vector<glm::ivec3> > createPolyhedron(int subdivisions, vector<glm::vec3>& vertices, vector<glm::ivec3> triangles);


		///-------------------------------------------
		// Members


		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		int iboID[1]; // Our Index  Object
		
		// the shader program that renders this object
		int program;


		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;

		vector<glm::vec3>		_points;
		vector<glm::vec3>		_colors;
		vector<glm::ivec3>		_triangle_index;
		int						_I; // num indices

		float	_radius;
		int		_level; 

private:

};

}//namespace arlab 