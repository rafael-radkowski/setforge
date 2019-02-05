#include "NormalMapSobel.h"


/*
Estimate a normal map from an RGB image
@param src - the source image - should be of type CV_8UC3
@param dst - the destination image of type CV_32FC3
@param kernel_size - the size of the sobel kernel - default is 2
@param filter_range - range for the bilateral filter to blend normal values
@return true - if successful
*/
//static 
bool NormalMapSobel::EstimateNormalMap(cv::Mat& src, cv::Mat& dst, int kernel_size, int filter_range)
{
	cv::Mat img = src.clone();
	cv::Mat img_filter, img_gray, sobelx, sobely;

	// sobel
	cv::bilateralFilter( img, img_filter, filter_range, 30, 50 );   // 30, 50 empirically found
	cv::cvtColor(img_filter, img_gray, cv::COLOR_RGB2GRAY);
	cv::Sobel(img_gray, sobelx, CV_64F, 1, 0, kernel_size);
	cv::Sobel(img_gray, sobely, CV_64F, 0, 1, kernel_size);

	int w = img_gray.cols;
	int h = img_gray.rows;

	// compute normals
	cv::Mat normal_map = cv::Mat::zeros(h, w, CV_32FC3);
	dst = cv::Mat::zeros(h, w, CV_32FC3);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			double sx = sobelx.at<double>(i, j);
			double sy = sobely.at<double>(i, j);

			double s = std::pow((double)sx, 2.0) + std::pow((double)sy, 2.0) + 1.0;
			s = std::sqrt((double)s);

			double nx = -sx / s;
			double ny = -sy / s;
			double nz = 1.0 / s;

			// normalize
			nx = (nx + 1.0) / 2.0;
			ny = (ny + 1.0) / 2.0;
			nz = (nz + 1.0) / 2.0;

			double length = nx * nx + ny * ny + nz * nz;
			nx = nx / length;
			ny = ny / length;
			nz = nz / length;

			normal_map.at<cv::Vec3f>(i, j) = cv::Vec3f((float)nz, (float)ny, (float)nx);
		}
	}

	cv::bilateralFilter( normal_map, dst, 3, 10, 10 );
	//dst = normal_map;
	return true;
}