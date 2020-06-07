#pragma once
/*
@class ControlPointsHelper
@brief	Helper class to write and read control points to and from files. 

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
#include <fstream>
#include <iostream>
#include <string>
#include <strstream>
#include <sstream>
#include <vector>
#include <list>
#include <numeric>

// glm
#include <glm\glm.hpp>

//local
#include "FileUtils.h"

class ControlPointsHelper {
public:
	typedef enum {
		NONE,
		BBox, // projected bounding box points
		BBoxLocal // unprojected bounding box points. 
	}CPType;

	/*!
	Read 2D points from a file 
	@param path_and_filename - the path and file for the points. 
	@param type - the control point type
	@param control_points - location of the points as glm::vec2 (u,v);
	@return true, if successful. 
	*/
	static bool Read(const std::string path_and_filename, CPType& type, std::vector<glm::vec2>& control_points);

	/*!
	Write 2D points to a file 
	@param path_and_filename - the path and file that stores the points. 
	@param type - returns the control point type
	@param control_points - location to store the points as glm::vec2 (u,v);
	@return true, if successful. 
	*/
	static bool Write(const std::string path_and_filename, const CPType type,  std::vector<glm::vec2>& control_points);

	/*!
	Read 3D points from a file 
	@param path_and_filename - the path and file for the points. 
	@param type - the control point type
	@param control_points - location of the points as glm::vec3 (x, y, z);
	@return true, if successful. 
	*/
	static bool Read3D(const std::string path_and_filename, CPType& type, std::vector<glm::vec3>& control_points);

	/*!
	Write 3D points to a file 
	@param path_and_filename - the path and file that stores the points. 
	@param type - returns the control point type
	@param control_points - location to store the points as glm::vec3 (x, y, z);
	@return true, if successful. 
	*/
	static bool Write3D(const std::string path_and_filename, const CPType type,  std::vector<glm::vec3>& control_points);


private:

	// helper to split a line using the delimiter
	static std::vector<string> split(std::string str, char delimiter);

};
