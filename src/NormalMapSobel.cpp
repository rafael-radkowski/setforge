#include "NormalMapSobel.h"

/* To activate some helper functions to write the 
output of this filter into files for documentation, debugging, etc. 
*/
//#define WRITE_OUTPUT

#ifdef WRITE_OUTPUT
int scount = 0;
#endif
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

#ifdef WRITE_OUTPUT
	cv::imwrite("normal_maps/normal_1" + to_string(scount) + ".png", src);
	cv::imwrite("normal_maps/sobelx_1" + to_string(scount) + ".png", sobelx);
	cv::imwrite("normal_maps/sobely_1" + to_string(scount) + ".png", sobely);
#endif


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

#ifdef WRITE_OUTPUT
	cv::Mat normals_16UC3;
	normal_map.convertTo(normals_16UC3, CV_16UC3, 65535 );

	cv::imwrite("normal_maps/normal_map" + to_string(scount) + ".png", normals_16UC3);
	cv::imshow("normal_map", normals_16UC3);
	cv::waitKey();

	scount++;
#endif

	//dst = normal_map;
	return true;
}