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

April 20, 2020, RR
- Added a class and function to render random colors. 

April 21, 2020, RR
- Added a class that reads material data from a file. 
- Added the api void applyRandomColor(void) to clean code

May 9, 2020, RR
- Integrated the the BRDF renderer to become a part of the ModelRenderer class.

June 5, 2020, RR
- Added a bounding box model to the renderer
- Added an api to enable/disable bounding box rendering.
June 6, 2020, RR
- Added a function to project bounding box corner points and to store those to a file.

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
#include "ModelBRDF.h" // brdf model
#include "ShaderProgram.h"
#include "CommonTypes.h"  
#include "RenderToTexture.h"
#include "ModelPlane.h"
#include "ImageWriter.h"
#include "ModelCoordinateSystem.h"
#include "RoIDetect.h"
#include "ImageMask.h"
#include "GLSLShaderSrc.h"
#include "MaterialRandomization.h"  // for random colors
#include "MaterialReaderWriter.h"  // to read material data from a fle. 
#include "ModelBBox.h"	// boudning box
#include "PointProjection.h" // point projection;

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
	Load the model that needs to be rendered. This model uses brdf material. 
	@param path_and_file - string containg the relative or absolute
							path to the model.
	@param brdf_material - brdf material object. 
	@return - true, if model was successfully loaded. 
	*/
	bool setModel(string path_and_file, cs557::BRDFMaterial& brdf_material);


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


	/*
	Enable or disable random color rendering. 
	@param enable - true enables random colors. 
	*/
	void setRandomColors(bool enable );


	/*
	Set the min / max values for hue and saturation
	@param hue_min - a min. hue value in the range from 0 to 360 degree.
	@param hue_max - a max. hue value in the range from 0 to 360 degree.
	@param sat_min - a min. saturation value in the range from 0 to 1.
	@param sat_max - a max. saturation value in the range from 0 to 1.
	@param v_min - a min. birhgntess v value in the range from 0 to 1.
	@param v_max - a max. birhgntess v value in the range from 0 to 1.
	@param with_v enable or disable v independently. True enables a randomized v. 
	*/
	void setRandomColorsParams(float hue_min, float hue_max, float sat_min, float sat_max, float v_min = 0.8, float v_max = 0.8, float with_v = false);


	/*
	Enable or disable the bounding box renderer.
	@param draw - true enables bounding box rendering. 
	*/
	void setDrawBBox(bool draw);


	/*
	Enable or disable the bounding box projection function. 
	It is enabled by default. 
	*/
	void withBBoxProjection(bool enable);

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


	/*
	Project the bounding box manually. 
	This is a debug function to verify that the bounding box works. 
	*/
	bool projectBBox(void);
	

private:


	/*
	Draw the scene into an fbo to get textures
	*/
	bool drawFBO(void);


	/*
	Project the boundinx box corner points and the bounding box centroid. 
	*/
	bool projectBBoxPoints();


	/*
	Create a scene for the prerenderer
	*/
	void CreatePrerendererScene(void);

	/*
	Creates some helper displays
	*/
	void CreateHelperContent(void);


	/*
	Applies random color to the model if 
	the setting is set to random color. 
	*/
	void applyRandomColor(void);


	// members

	// the model to render
	cs557::OBJModel*			_obj_model;
	cs557::OBJModel*			_obj_model_normals;
	cs557::BBox*				_bbox;

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

	// projected control points.
	// if the bounding box is projected, the vector contains 8 corner points
	// and the center. 
	std::vector<glm::vec2>	_projected_points;

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
	bool					_with_bbox; // renders a bounding box;
	bool					_with_bbox_projection; // enables the bounding box projection

	// For random colors
	MaterialRandomization	_rand_col;
	bool					_with_rand_col;

	// point projection
	PointProjection*		_projection;

protected:

	bool						_verbose;
};