#pragma once
/*
class NormalMapSobel

This class estimates a normal map from a single RGB images.
It uses a Sobel operator to compute gradients. The gradients are then used to further
estimate normal vectors. 

Note that the normal vectors are not optimal. But it is data.
The normal maps are scaled to [0,1]

A bilateral filter is used to smooth the image.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Jan 25, 2019
MIT license
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

using namespace std;

class NormalMapSobel
{
public:


	/*
	Estimate a normal map from an RGB image
	@param src - the source image - should be of type CV_8UC3
	@param dst - the destination image of type CV_32FC3
	@param kernel_size - the size of the sobel kernel - default is 2
	@param filter_range - range for the bilateral filter to blend normal values
	@return true - if successful
	*/
	static bool EstimateNormalMap(cv::Mat& src, cv::Mat& dst, int kernel_size = 3, int filter_range = 25);



};