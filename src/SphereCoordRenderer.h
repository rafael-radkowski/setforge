#ifndef __SPHERECOORDINATE_RENDERER__
#define __SPHERECOORDINATE_RENDERER__
/*
class SphereCoordRenderer


Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved
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

#include "ModelRenderer.h"

class SphereCoordRenderer : public ModelRenderer
{
public:

	/*
	Constructor
	@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
	@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
	*/
	SphereCoordRenderer(int window_width = 1280, int window_height = 1024, int image_width = 2560, int image_height = 2048);
	~SphereCoordRenderer();

	/*
	Create  the number of segments and rows of the sphere.
	This defines the number of camera points. 
	@param radius - the sphere radius. This param sets the distance between camera and the 3D model 
	@param segments - the number of vertical segments
	@param rows - the number of horizontal rows 
	*/
	void createSphereGeometry( float radius, int segments, int rows);

	/*
	Draw the image sequence and save all images to a file
	@return - false, if images still need to be rendered. 
			  true, id all images were saved to a file. 
	*/
	bool draw_sequence(void);


private:


	void SphereCoordRenderer::make_geometry(glm::vec3 center, double radius, double segments, double rows,
		std::vector<glm::vec3> &spherePoints, std::vector<glm::vec3> &normals);


	//--------------------------------------------------------------
	// members

	int					_segments;
	int					_rows;
	double				_radius;
	glm::vec3			_center;
	glm::vec3			_up;

	float				_camera_distance; 

	int					_N; // max number of points.
	int					_N_current; // current point to render

	std::vector<glm::vec3> _points;
	std::vector<glm::vec3> _normals;

};




#endif