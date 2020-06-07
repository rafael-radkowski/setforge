#pragma once
/*
@class GLGeometryUtils
@brief	The class provides function to calculate the axis-aligned bounding box of an object and
		the objects centroid. It also allows one to move an 3D model's centroid to the center of 
		the bounding box.

		p7 /---------------------/p6
		  /					    /|
		 /					   / |
	 p3	/---|-----------------/ p2
		|	/				  |  /p5
		|	p4				  | /
		|					  |/
		|---------------------/
		p0					 p1

	  z	|
		/-> y
	   x
Features:
- Compute the bounding box of a set of 3D points.
- Compute the centroid of the points. 


Rafael Radkowski
Iowa State University
rafael@iastate.edu
515 294 7044
MIT License
-------------------------------------------------------------------------------------------------------
Last edited:

June 6, 2020, RR
- Added functions to compute and return bounding box corners


*/

// stl
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// glm
#include <glm/glm.hpp>

// local


class GLGeometryUtils
{
public:

	/*!
	Calculate the bouding box size of the object. 
	@param points - location of a vector with points (x, y, z) in glm::vec3 format. 
	@param boundingbox_size - the length of the bounding box sizes in local object coordinates.
	@return true - if successfully calculated. 
	*/
	static bool CalcBoundingBox(std::vector<glm::vec3>& points, glm::vec3& boundingbox_size);

	/*!
	Calculate the bouding box size of the object. 
	@param points - location of a vector with points and textur coordinates: ((x, y, z)(u, v)>.
		The function splits up the vector and processes the points. 
	@param boundingbox_size - the length of the bounding box sizes in local object coordinates.
	@return true - if successfully calculated. 
	*/
	static bool CalcBoundingBox(std::vector<std::pair<glm::vec3, glm::vec2> > & points, glm::vec3& boundingbox_size);


	/*
	Identify the centroid of the object. 
	@param points - location of a vector with points (x, y, z) in glm::vec3 format. 
	@param centroid - location to store the coordinates of the centroid.
	@return true - if successfully calculated. 
	*/
	static bool CalcGeometricCentroid(std::vector<glm::vec3>& points, glm::vec3& centroid);
	

	/*
	Determine the bounding box of the object and its geometric centroid. 
	@param points - location of a vector with points and textur coordinates: ((x, y, z)(u, v)>.
		The function splits up the vector and processes the points. 
	@param boundingbox_size - the length of the bounding box sizes in local object coordinates.
	@param centroid - location to store the coordinates of the centroid.
	@return true - if successfully calculated. 
	*/
	static bool CalcAll(std::vector<std::pair<glm::vec3, glm::vec2> > & points, glm::vec3& boundingbox_size, glm::vec3& centroid);



	/*
	Return the bounding box corners of the last calculated bounding box. 
	@return  the eight bounding box corners in local x, y, z, coordinates. 
	*/
	static std::vector<glm::vec3>& GetBBCorners(void);


	/*
	Set the 3D model's center to the center of the bounding box. 
	@param points - location of a vector with points (x, y, z) in glm::vec3 format. 
		The function manipulates the point vector directly. 
	*/
	static bool SetToBBCenter(std::vector<glm::vec3>& points);
	static bool SetToBBCenter(std::vector<std::pair<glm::vec3, glm::vec2> > & points);

	/*
	Enable additional ouput
	@param verbose - true enables additional console infos and warnings. 
	*/
	static bool SetVerbose(bool verbose);


private:
	
	/*
	Calcualte the bounding box corner points p0 to p7
	*/
	static void CalculateBBCorners(void);

};