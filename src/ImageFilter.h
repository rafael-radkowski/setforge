#pragma once
/**
* This class is built for Chromatic adaptation
* Chromatic adaptation is the human visual systemm's ability to adjust
* to changes in illumination in order to preserve the appearance of
* object colors.
* There are 3 Chromatic adaptation algorithms: 
* Gray World Algorithm, White Balance Algorithm, Adaptation for template image
*/

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;


class ImageFilter {

public:
	/**
	* Supported Chromatic Adaptation Algorithms
	*
	* These values are used for setting which Chromatic Adaptation Algorithm
	* will be used in getMat(Mat input) function
	*/
	typedef enum {
		GRAYWORLD,
		WHITEBALANCE,
		CHROMATIC
	} FilterMethod;

	/**
	* Constructor for ImageFilter
	* Default Chromatic Adaptation Algorithm is Gray World Algorithm
	*/
	ImageFilter();


	/**
	* Deconstructor for ImageFilter
	*/
	~ImageFilter();

	

	/**
	* Set the Chromatic Adaptation Algorithm to Template Image Adaptation
	*/
	void setChromaticTemplate(cv::Mat& template_img);

	/**
	* Set the Chromatic Adaptation Algorithm to Gray World Algorithm
	*/
	void setFilterMethod(FilterMethod method);


	/*!
	Apply the filter currently set. Note that the filter
	expects to get an RGB image. It processes all three channels individually and merges them when finished/ 
	@param input - A CV_8UC3 image. 
	@return - A CV_8UC3 image with the  resolution of the input image
	*/
	bool apply(cv::Mat& input, cv::Mat& dst);



private:


	/*
	Apply the grayworld filter. It creates a gray-tone image.
	@param input - A CV_8UC3 image. 
	@return - A CV_8UC3 image with the  resolution of the input image
	*/
	cv::Mat grayworldFilter(cv::Mat& src);

	/*
	Apply a white-balance filter. 
	@param input - A CV_8UC3 image. 
	@return - A CV_8UC3 image with the  resolution of the input image
	*/
	cv::Mat whitebalanceFilter(cv::Mat& src);

	/*
	Apply a choromatic filter. It adapts the color tone and hue to a 
	given template. Use setChromaticTemplate() to set the template. 
	The function will not proceed without a template. 
	*/
	cv::Mat chromaticFilter(cv::Mat& src);


	// Variable used for define the algorithm type
	FilterMethod	_method;

	// Mat variable used for save template image for chromatic adaptation
	Mat				_template_img;

};