#include "RoIDetect.h"


/*
Detect a region of interest in an RGB rendering.
Note that this is a primitive function which assumes that only one 
rendered object is shown in the image in front of a black background, RGB = (0,0,0).
@param image - a reference to the image.
@param x - returns the x start (columns) coordinate of the roi
@param y - returns the y start coordinate (rows) of the roi
@param dx - returns the width of the roi
@param dy - returns the hieght of the roi
*/
//static 
bool RoIDetect::Extract(cv::Mat& image, cv::Rect2f& roi )
{

	int x; 
	int y;
	int width; 
	int height;
	x = image.cols;
	y = image.rows;
	width = 1;
	height = 1;

	cv::Vec3b last_pixel(0, 0, 0);
	int xr = 0;
	int yd = 0;

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			cv::Vec3b col = image.at<cv::Vec3b>(i, j);
			// search for the first not 0 pixel
			bool all_zero = false;
			if (col[0] > 0.0 || col[1] > 0.0 || col[2] > 0.0) {
				all_zero = true;
				if (j < x) {
					x = j;
				}
				if (i < y) {
					y = i;
				}
			}
			// search for the last not zero pixel
			if (last_pixel[0] > 0 || last_pixel[1] > 0 || last_pixel[2] > 0) {
				if (all_zero == true) {
					if (j > xr) {
						xr = j;
					}
					if (i > height + y) {
						height = i - y;
					}
				}
			}




			last_pixel = col;
		}

	}
	width = xr - x;

	x = std::max((int)0, std::min((int)x, (int)image.cols));
	y = std::max((int)0, std::min((int)y, (int)image.rows));

	width = std::max(1, std::min(width, image.cols));
	height = std::max(1, std::min(height, image.rows));

	roi.x = x;
	roi.y = y;
	roi.width = width;
	roi.height = height;

#ifdef DEBUG
	cv::Mat clone = image.clone();
	cv::rectangle( clone, cv::Point(x, y), cv::Point(x + width, y + height), cv::Scalar(0,255,255), 1);
	cv::imshow("Region", clone);
	cv::waitKey(1);
#endif

	return true;
}



/*
Render the RoI
@param image - a reference to the image.
@param roi - the region of interest. 
*/
//static 
bool RoIDetect::RenderRoI(cv::Mat& image, cv::Rect2f roi)
{

	float scalex = (float)512 / (float)image.cols;
	float scaley = (float)512 / (float)image.rows;

	int px = (float)roi.x * scalex;
	int py = (float)roi.y * scaley;
	int wx = (float)roi.width * scalex;
	int wy = (float)roi.height * scaley;

	cv::Mat output_resized;
	cv::Mat output = image.clone();
	cv::resize(output, output_resized, cv::Size(512, 512));
	cv::rectangle( output_resized, cv::Point(px, py), cv::Point(px + wx, py + wy), cv::Scalar(0,255,255), 2);


	std::string str = "x: ";
	str.append(std::to_string(std::floor(roi.x)));
	str.append(", y: ");
	str.append(std::to_string(std::floor(roi.y)));

	std::string str2 = "w: ";
	str2.append(std::to_string(std::floor(roi.width)));
	str2.append(", h: ");
	str2.append(std::to_string(std::floor(roi.height)));

	cv::putText(output_resized, str, cvPoint(10,20), 
    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);
	cv::putText(output_resized, str2, cvPoint(10,40), 
    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);
	cv::imshow("Region of Interest", output_resized);
	cv::waitKey(1);

	return true;
}