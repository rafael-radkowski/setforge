#ifndef __ROIDETECT__
#define __ROIDETECT__
/*
class RoIDetect

This class extract the region of interest of a rendered object. 
The algorithm is quite simple. 
First, it assumes that the background is black. 
Thus, it detects the region of interest as the first and last NOT-BLACK pixel 
in all rows and columns. 

The algorithm is also expensive. So it may needs optimization in future. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
May 2019
All copyrights reserved
----------------------------------------------------------------------------------------
Last edited:

July 4th, 2019, RR
- Fixed a bug that displayed an incorred ROI when image rows != cols
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


class RoIDetect 
{
public:

	/*
	Detect a region of interest in an RGB rendering.
	Note that this is a primitive function which assumes that only one 
	rendered object is shown in the image in front of a black background, RGB = (0,0,0).
	@param image - a reference to the image of type CV_8UC3.
	@param roi - location to return the region of interest. 
	*/
	static bool Extract(cv::Mat& image, cv::Rect2f& roi);


	/*
	Render the RoI
	@param image - a reference to the image.
	@param roi - the region of interest. 
	*/
	static bool RenderRoI(cv::Mat& image, cv::Rect2f roi);
};



#endif