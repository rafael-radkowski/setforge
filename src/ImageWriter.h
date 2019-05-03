#ifndef __IMAGEWRITER__
#define __IMAGEWRITER__
/*
class ImageWriter

The class OpenCV Mat images to .png files as well as the pose of an object as matrix to a .txt file.
It also maintains a log file to summarize all writing tasks. The log file is overwritten each time an instance of
this class is created. 

Features:


Usage:


Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved

------------------------------------------------------------------
last edited:
Jan 21, 2019:
- Added the pose and orientation of the model to the log file
- Added a variable for the log file to the class
- Fixed a bug that prevented that the class cleans the output folder. 

May 2, 2019, RR
- Added a new function to write data to a file. This new write function stores additional data and uses a struct as datatype. 
May 3, 2019, RR
- Changed the csv parameter to roi_x, roi_y, roi_w, roi_h to align all tools.
*/

// stl
#include <iostream>
#include <vector>
#include <string>
#include <experimental/filesystem>

// opencv
#include <opencv2/opencv.hpp>

// Eigen
#include <Eigen/Dense>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// local
#include "MatrixFileUtils.h"
#include "MatrixHelpers.h"
#include "types.h"

using namespace std;

class ImageWriter
{
public:

	typedef struct IWData {
		int index;
		cv::Mat* rgb;
		cv::Mat* normals;
		cv::Mat* depth;
		cv::Mat* mask;
		cv::Rect2f roi;
		glm::mat4 pose;


		IWData(){
			index = 0;
			roi.x = 0;
			roi.y = 0;
			roi.width = 0;
			roi.height = 0;

			rgb = NULL;
			normals = NULL;
			depth = NULL;
			mask = NULL;
		}

	}IWData;



	ImageWriter();
	~ImageWriter();


	/*
	Set the output path and name
	@param path - the output file path
	@param name - the file name as string
	*/
	void setPathAndImageName(string path, string name);


	/*
	Write the image data to a file
	@param index - the image id;
	@param rgb - cv::Mat& of type CV_8U_C3 with the rbg image content
	@param normals - cv::Mat of type CV_32F_C3 with normal vectors 
	@param depth - cv::Mat of type CV_32F_C1 with the depth values
	@para pose - the current camera pose
	*/
	bool write(int index, cv::Mat& rgb, cv::Mat& normals, cv::Mat& depth, glm::mat4 pose);


	/*
	Write the image data to a file
	@param data - a dataset of type IMData
	*/
	bool write(IWData& data);



private:

	/*
	Check whether the path exists. 
	Create a folder if the path does not exist. 
	@param path - string with the relative or absolute path. 
	*/
	bool checkFolder(string path);


	//---------------------------------------------------------
	// members

	string					_output_file_path;
	string					_output_file_name;

	string					_logfile_name;



};


#endif