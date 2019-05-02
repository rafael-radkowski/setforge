#ifndef __IMAGEMASK__
#define __IMAGEMASK__
/*
class ImageMask

This class creates a mask separating the rendering from the background.
The algorithm uses a binary threshold function. 
It assumes that the entire background is black RGB = (0,0,0).

The mask is of type CV_8UC1

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
May 2019
All copyrights reserved
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>

// opencv
#include <opencv2/opencv.hpp>

// local 
#include "types.h"

class ImageMask 
{
public:

	/*
	Extracts the mask from a depth image
	@param image - a reference to the image of type CV_8UC3.
	@param mask - location to return the mask as CV_8UC1.
	*/
	static bool Extract(cv::Mat& image, cv::Mat& mask_out);



};



#endif