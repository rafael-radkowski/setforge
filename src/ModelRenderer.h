#pragma once
/*
class ModelRenderer

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
Jan 19, 2019, RR
- added a normal map to the renderer. 

May 2nd, 2019, RR
- Added a region of interest detection. A simple algorithm that finds the roi using the RGB image. 
- Added a function to extract an image mask from the rendered color image by separating foreground from background. 
- Changed the ImageWriter->write function this class uses. The new version uses a struct as datatype.
*/

// stl
#include <iostream>
#include <vector>
#include <string>

// opencv
#include <opencv2/opencv.hpp>


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
#include "RenderToTexture.h"
#include "ModelPlane.h"
#include "ImageWriter.h"
#include "ModelCoordinateSystem.h"
#include "RoIDetect.h"
#include "ImageMask.h"
#include "GLSLShaderSrc.h"

using namespace std;

class ModelRenderer {

public:
	/*
	Constructor
	@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
	@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
	*/
	ModelRenderer(int window_width = 1280, int window_height = 1024, int image_width = 2560, int image_height = 2048 );
	~ModelRenderer();


	/*
	Set the model to render
	@param path_and_file - string containg the relative or absolute
							path to the model.
	@return - true, if model was successfully loaded. 
	*/
	bool setModel(string path_and_file);


	/*
	Draw the current object in a window and into the fbo
	*/
	bool draw(void);


	/*
	Set the intrinsic parameters
	@param  fx, fy, Field of view in pixels into the x and y direction. 
	@parm	px, py, the camera principle point
	*/
	void setIntrinsic(float fx, float fy, float px, float py);

	/*
	Set the view matrix. 
	@param viewmatrix - a 4x4 view matrix. 
	*/
	void setCameraMatrix(glm::mat4 viewmatrix);

	/*
	Set an output path for the images
	@param path - relative or absolute output path
	@param name - name template for the output images. 
	*/
	void setOutputPath(string path, string name = "model");

	/*
	Enable additional output 
	*/
	void setVerbose(bool verbose) { _verbose = verbose; };


	/*
	Return the number of generated images
	*/
	int size(void) { return _output_file_id; }


protected:

	/*
	Draw the current object into a window and into the fbo.
	Save the fbo to file
	*/
	bool draw_and_save(void);

	/*
	Disable and enable the file writer
	*/
	bool enable_writer(bool enable);

private:


	/*
	Draw the scene into an fbo to get textures
	*/
	bool drawFBO(void);
	
	/*
	Create a scene for the prerenderer
	*/
	void CreatePrerendererScene(void);

	/*
	Creates some helper displays
	*/
	void CreateHelperContent(void);


	// members

	// the model to render
	cs557::OBJModel*			_obj_model;
	cs557::OBJModel*			_obj_model_normals;

	glm::mat4				_projectionMatrix;		
	glm::mat4				_viewMatrix;			
	glm::mat4               _modelMatrix; 


	// The light source
	cs557::LightSource		_light0;
	cs557::LightSource		_light1;
	cs557::LightSource		_light2;

	// Material
	cs557::Material	 		_mat0;

	// preremder scene
	unsigned int				_fboHidden;
	GLuint					_color_texture_idx;
	GLuint					_depth_texture_idx;

	// to render normal vectors
	unsigned int				_fboHiddenNormals;
	GLuint					_normal_texture_idx;
	GLuint					_normal_depth_texture_idx;

	// width and heigh of the current GL output window
	int						_width;
	int						_height;

	// width and height the fbo should render with.
	// The output image has the same width and height. 
	int						_image_width;
	int						_image_height;

	// pointer for memory to retrieve the RGB and depth data from the fbo
	unsigned char*			_data_rgb;
	unsigned char*			_data_depth;
	unsigned char*			_data_normals;

	// for helpers
	// a helper plane to render the pre-render content
	cs557::Plane				_display;
	glm::mat4				_display_m;
	glm::mat4				_display_p; // Store the projection matrix
	glm::mat4				_display_v;       // Store the view matrix

	bool						_save;
	int						_output_file_id;
	string					_output_file_path;
	string					_output_file_name;

	ImageWriter*				_writer;
	bool						_writer_enabled;

	// a corodinate system
	cs557::CoordinateSystem     _coordinateSystem;
	glm::mat4                   _modelMatrixCoordSystem; // for the coordinate system


	bool					_with_roi; // also extracts the roi from the color image
	bool					_with_mask;//  extracts the mask from the depth image

protected:

	bool						_verbose;
};