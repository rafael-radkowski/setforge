#pragma once
/*
@class PointProjection
@brief	The class projects 3D points onto a 2D plane using a standard CG perspective camera projection. 
		The input are points in 3D space, and the output are projected points in image space in pixels. 

		Image coordinates
		-----> u-------------
		|					|
	   \|/v					|
	    |					|image_height
		|					|
		---------------------
		image_width

		Control points that are out of frame are set to -1.
Input:
	- Image width and height in pixel
	- View matrix
	- projection matrix. 
	- Control points

Output:
	- Proejcted control points in image space. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
(515) 294 7044
rafael@iastate.edu
MIT License
----------------------------------------------------------------------------------------------------------------------
Last edits:


*/

// stl
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// glm
#include <glm/glm.hpp>

// opencv
#define _WITH_OPENCV
#ifdef _WITH_OPENCV
#include <opencv2\opencv.hpp>
#endif

class PointProjection
{
public:

	/*
	Constructor
	@param image_width, image_height - the width and height of the output image/rendered image in pixel. 
	*/
	PointProjection(int image_width, int image_height);
	~PointProjection();


	/*!
	Set the opengl view matrix
	@param view_matrix - an opengl 4x4 view matrix.
	*/
	void setViewMatrix(glm::mat4 view_matrix);
	
	/*!
	Set the opengl projection matrix. 
	@param proj_matrix - a 4x4 opengl projection matrix. 
	*/
	void setProjectionMatrix(glm::mat4 proj_matrix);


	/*
	Project a set of 3D points to a 2D plane with the given view matrix and projecton matrix. 
	@param points - a vector with N points in x, y, z, coordinates. 
	@param matrix - a 4x4 transformation matrix to move the points.
	@param projected_points - the projected output points in u, v coordinate in image coordinate space. 
	*/
	void projectPoints(const std::vector<glm::vec3> points, const glm::mat4 matrix, std::vector<glm::vec2>& projected_points);


	/*
	Enable the debug window showing the points. 
	*/
	void setVerbose(bool verbose);


	/*!
	Render an opencv image that shows the LAST projected points. 
	*/
	void showProjection(void);

private:

	// image size in pixel
	int			_img_width;
	int			_img_height;
	
	// opengl projection and view matrix. 
	glm::mat4	_proj_matrix;
	glm::mat4	_view_matrix;

	// the last projected points
	std::vector<glm::vec2> _projected_points;

	bool		_verbose;
};