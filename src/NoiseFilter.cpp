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

AddNoise::AddNoise()
{
	
}

/**
* Add Gaussian Noise for RGB CV_8U or CV_16U 3-channel-images
*
* @param img Input RGB CV_8U or CV_16U 3-channel-images.
* @param mean Mean of Gaussian noise
* @param sigma Standard deviation of Gaussian noise. range from 0 to 1.
* @return noise  Image with guassian noise
*/
cv::Mat AddNoise::addGuasNoise(cv::Mat img, float mean, float sigma) {
	Mat noise;
	noise = img.clone();
	RNG rng;
	sigma = sigma * 255;
	rng.fill(noise, RNG::NORMAL, mean, sigma);
	add(noise, img, noise);

	return noise;
}

/**
* Add Speckle Noise for RGB CV_8U or CV_16U 3-channel-images
*
* @param img Input RGB CV_8U or CV_16U 3-channel-images.
* @param dev Standard deviation of speckle noise. range from 0 to 1.
* @return noise  Image with speckle noise
*/
cv::Mat AddNoise::addSpeckleNoiseRGB(cv::Mat img, float dev) {
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

AddNoise::~AddNoise()
{

}
