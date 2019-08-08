/**
* Add Guassian Noise or Speckle Noise for 3-chanel-images
*
*/
#include "NoiseFilter.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <Eigen/Eigen>

using namespace cv;
using namespace std;



/**
* Add Gaussian Noise for RGB CV_8U or CV_16U 3-channel-images
*
* @param img Input RGB CV_8U or CV_16U 3-channel-images.
* @param mean Mean of Gaussian noise
* @param sigma Standard deviation of Gaussian noise. range from 0 to 1.
* @return noise  Image with guassian noise
*/
cv::Mat NoiseFilter::AddGaussianNoise(cv::Mat img, float mean, float sigma) {
	Mat noise;
	noise = img.clone();
	RNG rng;
	sigma = sigma * 255;

	// generate noise
	rng.fill(noise, RNG::NORMAL, mean, sigma);

	// create a mask to make sure to only affect the area with color and not the empty background. 
	cv::Mat grayscaleMat, mask;
	cvtColor(img, grayscaleMat, CV_RGB2GRAY);
    cv::threshold(grayscaleMat, mask, 0.0, 255, CV_THRESH_BINARY); // 0.0 -> black background

	// add the noise
	cv::Mat out;
	add( noise, img, out, mask);

	return out;
}

/**
* Add Speckle Noise for RGB CV_8U or CV_16U 3-channel-images
*
* @param img Input RGB CV_8U or CV_16U 3-channel-images.
* @param dev Standard deviation of speckle noise. range from 0 to 1.
* @return noise  Image with speckle noise
*/
cv::Mat NoiseFilter::AddSpeckleNoiseRGB(cv::Mat img, float dev) {
	Mat res;
	res = img.clone();
	CvMat* pNoise = cvCreateMat(img.rows, img.cols, CV_32F);
	CvRNG rng(6);

	cout << img.type() << endl;

	cvRandArr(&rng, pNoise, CV_RAND_NORMAL, 1, dev);
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			float tmp = pNoise->data.fl[img.cols*i + j];
			res.at<Vec3b>(i,j) = img.at<Vec3b>(i,j) * tmp;
		}
	}
	return res;
}

