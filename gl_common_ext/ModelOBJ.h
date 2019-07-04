/*
ModelOBJ - This code is part of CS/CPRE/ME 557 - Computer Graphics 

This class provides functions to load and render an 3D model stored as .obj file. 
However, it just loads the meshes from an obj file and renders them. 
Materials and a shader program must be manually assigned. 

Features:
- Load all meshes from an obj file. 
- Draws the meshes (if a shader program was assigned)

Rafael Radkowski
Iowa State Univerity
rafael@iastate.edu
Sep. 12, 2018
rafael@iastate.edu

Last edit:
Dec 16, 2018, RR
	- fixed a bug that skipped severa triangles from being rendeered.
	- changed the code to process all data. 
Feb 19, 2019, RR
	- added material: The obj material from a mat file was not used correctly. 
		Added a feature to process it per mesh. 
July 4th, 2019, RR
	- Added a function to process diffuse, specular, and ambient textures. 
	- Added an api to change the texture blend mode e.g, MODULATE, DECAL 
*/
#pragma once
#include "OBJLoader.h"


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
#include "CommonTypes.h"  
#include "TextureLoader.h"
#include "Texture2D.h"
#include "../src/TextureLoader.h"

using namespace std;


namespace cs557
{

	class OBJModel {

	public:
		/*
		Load an OBJ model from file
		@param path_and_filename - number of rows
		@param shader_program - overwrite the default shader program by passing a hander to the constructor
		*/
		void create(string path_and_filename, int shader_program = -1);


		/*
		Draw the obj model
		@param viewMatrix - a view matrix object
		@param modelMatrix - a model matrix object.
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix);


		/*
		Return the shader program
		@return - int containing the shader program
		*/
		int getProgram(void) { return program; }


		/*
		Set the texture parameters.
		@param blend_mode - set the texture blend mode. MODULATE and REPLACE are currently supported. 
		*/
		void setTextureParam(TextureMode blend_mode);

	private:

		/*
		Process the diffuse, ambient, and specular texture of the object. 
		The function loads the textures from a file (using OpenCV), creates the texture object, 
		and copies the textures to the gpu.
		@param program - the shader program for this object. 
		@param curMesh - the current mesh that this instance creates.
		@param path - the path and file from which this object gets loaded. The function extracts the path.
		*/
		void processTextures(int& program, objl::Mesh& curMesh, string path);


		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		int iboID[1]; // Our Index  Object
		int program;
		int program_backup;

		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;

		// indices to render
		std::vector<int>		start_index;
		std::vector<int>		length;

		std::vector<cs557::Material>		materials;//material per mesh
		std::vector < cs557::TexMaterial>	textures;// textures per mesh

		int _N; // number of vertices
		int _I; // number indices
	};
}