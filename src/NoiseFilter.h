#pragma once
#include <opencv2/opencv.hpp>

/**
* Add Gaussian Noise or Speckle Noise for RGB CV_8U or CV_16U 3-channel-images
*
*/
class NoiseFilter
{
public:
	

	/**
	* Add Gaussian Noise for RGB CV_8U or CV_16U 3-channel-images
	*
	* @param img - Input RGB CV_8U or CV_16U 3-channel-images.
	* @param mean - Mean of guassian noise
	* @param sigma - Standard deviation of Gaussian noise. range from 0 to 1.
	* @return noise  Image with Gaussian noise
	*/

	static cv::Mat AddGaussianNoise(cv::Mat img, float mean, float sigma);
	/**
	* Add Speckle Noise for RGB CV_8U or CV_16U 3-channel-images
	*
	* @param img Input RGB CV_8U or CV_16U 3-channel-images.
	* @param dev Standard deviation of speckle noise. range from 0 to 1.
	* @return noise  Image with speckle noise
	*/
	static cv::Mat AddSpeckleNoiseRGB(cv::Mat img, float dev);
	

};