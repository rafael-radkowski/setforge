/*
class ImageLogReader

This class reads the image log file that the class ModelRenderer writes. 
The data is provided in a vector storing each dataset of type ImageLog.

It expects the data to be comma separated (.csv-file) 
index, rgb_file, normals_file, depth_file, mat_file, tx, ty, tz, qx, qy, qz, qw

TODO:
This class should be cobmbined with ImageWriter to one class, since both do similar things.

Usage:
 ImageLogReader:Read( path, &container)

 Rafael Radkowski
 Iowa State University
 rafael@iastate.edu
 Jan 9, 2019
 MIT License

 ----------------------------------------------------------------------------------------
 Last edits:

 Dec 10, 2019, RR:
 - Removed #incude <experimental/filesystem> since unused. 
*/
#pragma once

// stl
#include <iostream>
#include <string>
#include <strstream>
#include <vector>
#include <list>
#include <numeric>

// opencv
#include <opencv2/opencv.hpp>


// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// local
#include "BPTTypes.h"
#include "TimeUtils.h"

using namespace std;

class ImageLogReader
{
public:

	typedef struct _ImageLog{
		int id;
		string rgb_file;
		string normal_file;
		string depth_file;
		string maske_file;
		string matrix_file;

		glm::vec3	p;
		glm::quat	q;
		cv::Rect2f	roi;

		_ImageLog(int idx, string rgb, string normal, string depth, string mask, string matrix, glm::vec3 p_, glm::quat q_, cv::Rect2f roi_)
		{
			id = idx;
			rgb_file = rgb;
			normal_file = normal;
			depth_file = depth;
			maske_file = mask;
			matrix_file = matrix;
			p = p_;
			q = q_;
			roi = roi_;
		}

	}ImageLog;

	/*
	Read the dataset from a file.
	@param path_and_file - string containing the path and filename.
	@param log - reference to a container keepting the data. 
	*/
	static bool Read(string path_and_file, vector<ImageLog>* log);


private:

	/*
	Split a line into its segments
	@param str - string containing the line
	@param delimiter - the delimiter character. 
	*/
	static vector<string> split(string str, char delimiter);
};