#ifndef __RANDOMPOSEVIEW_RENDERER__
#define __RANDOMPOSEVIEW_RENDERER__
/*
class RandomPoseViewRenderer

This class renders images by selecting a random pose.  
The objects orientation is generated using the vertices of an polyhedron as camera locations.
Additionally, the object is moved in front of the camera to a random position. The user has to specify the limits.
The points of a polyhedron are uniform distributed. Thus, the delta between each camera is equal. 
Possible object's positions and orientations are stored in two lists and combined randomly during the rendering process. 

Camera coordinates:  view direction: -z,  up: y, side: x (to the right in image coordinates).

The renderer draws the rgb image, normal map, and depth map (linear) into a fbo and writes them into a file.

Set the number of subdivisions to generate a polyhedron (function create). 
Note that 0 subdivisions generates an Icosahedron.

Features:
- Renders the 3d model as RGB image and writes it into a file CV_8UC3)
- Renders the normal map and writes it into a file (CV_32FC1, float).
- Renders the depth map (linear) and writes the values into a file  (CV_32FC1, float).

Usage:

pose_renderer = new RandomPoseViewRenderer(1280, 1024, 1280, 1024);
pose_renderer->setVerbose(true); // set first to get all the output info
pose_renderer->setModel("path to model");
pose_renderer->setOutputPath("output");
pose_renderer->setPoseLimits(-2.0, 2.0, -2.0, 2.0, -1.0, 3.0);
pose_renderer->create(10000, 2 ); // number of images, polyhedron subdivisions, defines the orientation quantization.

Loop over 
pose_renderer->draw_sequence();
until the function returns 'true'. 'false' means that the sequence is incomplete. 


Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved

------------------------------------------------------------------
last edited:
Jan 21, 2019:
- Updated the up * eye  vector check to prevent that the view matrix becomes NaN. 
	Verified that the model is correctly oriented. 
Aug 8, 2019, RR
- Added a function that removes all canera viewpoints in the lower hemisphere of the polyhedron. 
	As a result, the 3D model will only be rendered in its upright position. 
*/


// stl
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <time.h>

// opencv
#include <opencv2/opencv.hpp>


// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

#include "PolyhedronGeometry.h" // for the Polyhedron geometry
#include "ModelRenderer.h"

class RandomPoseViewRenderer : public ModelRenderer
{
public:

	/*
	Constructor
	@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
	@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
	*/
	RandomPoseViewRenderer(int window_width = 1280, int window_height = 1024, int image_width = 2560, int image_height = 2048);
	~RandomPoseViewRenderer();

	/*
	Set the number of images to generate and the number of subdivisions for the polyheder
	The polyheder points define the camera view points. 
	@param num_images - the number of images to generate
	@param subdivisions - number of subdivisions. 0 is an Icosahedron
	*/
	void create( int num_images ,  int subdivisions = 4);


	/*
	Set the limits for the random pose in camera coordinates
	Camera coordinates:  view direction: -z,  up: y, side: x (to the right in image coordinates).
	@param nx, px - x-axis limits
	@param ny, py - y-axis limits 
	@param nz, pz - z-axis limits. Note that the camera looks into the negative z-direction.
					The limit for the z axis is currently set to -1.0.
	*/
	void setPoseLimits(float nx, float px, float ny, float py, float nz, float pz);


	/*
	Limits the orientation to upper hemisphere orientations only.
	Note that up means positive y direction
	@param upright - true to limit the orientations to upright poses. 
	*/
	void setHemisphere(bool upright);

	/*
	Draw the image sequence and save all images to a file
	@return - false, if images still need to be rendered. 
			  true, id all images were saved to a file. 
	*/
	bool draw_sequence(void);


private:

	/*
	Return a random position (translation) in camera space. 
	*/
	glm::mat4 getRandomPosition(void);



	//--------------------------------------------------------------
	// members

	int					_segments;
	int					_rows;
	double				_radius;
	glm::vec3			_center;
	glm::vec3			_up;

	float				_camera_distance; 

	int					_N; // max number of images to generate.
	int					_N_current; // current images rendered

	// the polyhedron results. Defines the camera viewpoins. 
	std::vector<glm::vec3> _points;
	std::vector<glm::vec3> _normals;

	bool				_upper_hemisphere; // if true, only camera views from the upper hemisphere will be rendered. 

	int						_subdivisions;
	float					_lim_px, _lim_nx;
	float					_lim_py, _lim_ny;
	float					_lim_pz, _lim_nz;

	std::random_device		_rd;
};




#endif